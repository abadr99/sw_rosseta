#include "frontend/CfgBuilder.hpp"

#include <map>
#include <set>
#include <sstream>
#include <vector>
#include <string>

#include "frontend/BasicBlock.hpp"
#include "utils/Macros.hpp"
#include "utils/Types.hpp"

using rosetta::frontend::basicblock::BasicBlock;
using rosetta::frontend::cfg::CfgBuilder;
using rosetta::frontend::cfg::ControlFlowGraph;
using rosetta::frontend::instruction::Instruction;
using rosetta::frontend::instruction::InstructionCategory;
using rosetta::frontend::instruction::OperandType;
using rosetta::utils::Address;

CfgBuilder::CfgBuilder(const std::vector<Instruction>& instructions)
  : instructions_(instructions) {
}

bool CfgBuilder::IsConditionalJump(const Instruction& instruction) const {
  return instruction.Category() == InstructionCategory::kCondControlFlow;
}

bool CfgBuilder::IsUnconditionalJump(const Instruction& instruction) const {
  return instruction.Category() == InstructionCategory::kUnCondControlFlow;
}

bool CfgBuilder::IsCall(const Instruction& instruction) const {
  return instruction.Category() == InstructionCategory::kCall;
}

bool CfgBuilder::IsReturn(const Instruction& instruction) const {
  return instruction.Category() == InstructionCategory::kReturn;
}

Address CfgBuilder::GetJumpTarget(const Instruction& instruction) const {
  const auto& operands = instruction.Operands();
  if (operands.empty()) {
    return 0;
  }

  const auto& op = operands.front();
  if (op.Type == OperandType::kImmediate) {
    return instruction.Address() + instruction.Size() + op.Imm;
  }
  return 0;
}

ControlFlowGraph CfgBuilder::Build() const {
  ControlFlowGraph graph;
  if (instructions_.empty()) {
    return graph;
  }

  // Find every address where a new block must start: the entry point,
  // every jump target, and every instruction right after a branch/return.
  std::set<Address> leaders = {instructions_.front().Address()};
  for (size_t i = 0; i < instructions_.size(); ++i) {
    const auto& inst = instructions_[i];
    bool is_branch = IsConditionalJump(inst) || IsUnconditionalJump(inst);

    if (is_branch) {
      leaders.insert(GetJumpTarget(inst));
    }
    if ((is_branch || IsReturn(inst)) && i + 1 < instructions_.size()) {
      leaders.insert(instructions_[i + 1].Address());
    }
  }

  // Split the instruction stream into basic blocks at those leaders.
  std::map<Address, BasicBlock> blocks;
  BasicBlock current;
  for (size_t i = 0; i < instructions_.size(); ++i) {
    current.AddInstruction(instructions_[i]);
    bool is_last = (i + 1 == instructions_.size());
    bool next_is_leader = !is_last &&
                          leaders.count(instructions_[i + 1].Address());

    if (is_last || next_is_leader) {
      blocks[current.StartAddress()] = current;
      current = BasicBlock();
    }
  }

  // Wire successor/predecessor edges based on each block's last instruction.
  for (auto& [start, block] : blocks) {
    const auto& last = block.InstructionList().back();
    Address fallthrough = last.Address() + last.Size();

    if (IsConditionalJump(last)) {
      Address target = GetJumpTarget(last);
      if (blocks.count(target)) {
        block.AddSuccessor(target);
        blocks[target].AddPredecessor(start);
      }
      if (blocks.count(fallthrough)) {
        block.AddSuccessor(fallthrough);
        blocks[fallthrough].AddPredecessor(start);
      }
    } else if (IsUnconditionalJump(last)) {
      Address target = GetJumpTarget(last);
      if (blocks.count(target)) {
        block.AddSuccessor(target);
        blocks[target].AddPredecessor(start);
      }
    } else if (!IsReturn(last) && blocks.count(fallthrough)) {
      block.AddSuccessor(fallthrough);
      blocks[fallthrough].AddPredecessor(start);
    }
  }

  for (const auto& [start, block] : blocks) {
    graph.AddBlock(block);
  }
  for (const auto& [start, block] : blocks) {
    for (Address successor : block.Successors()) {
      graph.AddEdge(start, successor);
    }
  }
  graph.SetEntryAddress(instructions_.front().Address());
  return graph;
}

std::string CfgBuilder::ToDot(const ControlFlowGraph& graph) const {
  std::ostringstream out;
  out << "digraph CFG {\n";
  out << "  node [shape=box, fontname=\"monospace\", fontsize=10];\n";

  int block_index = 0;
  for (const auto& [address, successors] : graph.BlocksList()) {
    const auto& block = graph.Block(address);
    const auto& insts = block.InstructionList();
    std::ostringstream label;

    label << "<b>Block " << block_index++ << 
      " (0x" << std::hex << address << "):</b><br align=\"left\"/>";

    if (insts.size() <= 6) {
      for (const auto& inst : insts) {
        label << inst.AssemblyText() << "<br align=\"left\"/>";
      }
    } else {
      for (size_t i = 0; i < 3; ++i) {
        label << insts[i].AssemblyText() << "<br align=\"left\"/>";
      }

      size_t omitted = insts.size() - 5;
      label << "<font color=\"#888888\"><i>" << 
        omitted << " instructions omitted</i></font><br align=\"center\"/>";

      for (size_t i = insts.size() - 2; i < insts.size(); ++i) {
        label << insts[i].AssemblyText() << "<br align=\"left\"/>";
      }
    }

    out << "  \"0x" << std::hex << address << "\" [label=<" << label.str() << ">];\n";
  }

  for (const auto& [address, successors] : graph.BlocksList()) {
    const auto& block = graph.Block(address);
    const auto& last = block.InstructionList().back();

    for (Address successor : successors) {
      std::string reason;
      std::string extra_attrs;
      if (IsConditionalJump(last)) {
        if (successor == GetJumpTarget(last)) {
          reason = "true";
          extra_attrs = ", color=green, fontcolor=green";
        } else {
          reason = "false";
          extra_attrs = ", color=red, fontcolor=red";
        }
      } else if (IsUnconditionalJump(last)) {
        reason = "jump";
        extra_attrs = ", color=blue, fontcolor=blue";
      } else {
        reason = "fallthrough";
      }

      out << "  \"0x" << std::hex << address << "\" -> \"0x" << std::hex << successor
          << "\" [label=\"" << reason << "\"" << extra_attrs << "];\n";
    }
  }
  out << "}\n";
  return out.str();
}
