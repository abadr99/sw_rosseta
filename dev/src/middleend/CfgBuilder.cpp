#include "middleend/CfgBuilder.hpp"

#include <map>
#include <set>
#include <vector>

#include "utils/Macros.hpp"

using rosetta::middleend::cfg::CfgBuilder;
using rosetta::middleend::basicblock::BasicBlock;
using rosetta::middleend::cfg::ControlFlowGraph;
using rosetta::frontend::decode::X86Instruction;
using rosetta::frontend::utils::Address;

CfgBuilder::CfgBuilder(const std::vector<X86Instruction>& instructions)
  : ICfgBuilder(instructions) {
}

bool CfgBuilder::IsConditionalJump(ZydisMnemonic mnemonic) const {
  switch (mnemonic) {
    case ZYDIS_MNEMONIC_JB:
    case ZYDIS_MNEMONIC_JBE:
    case ZYDIS_MNEMONIC_JCXZ:
    case ZYDIS_MNEMONIC_JECXZ:
    case ZYDIS_MNEMONIC_JKNZD:
    case ZYDIS_MNEMONIC_JKZD:
    case ZYDIS_MNEMONIC_JL:
    case ZYDIS_MNEMONIC_JLE:
    case ZYDIS_MNEMONIC_JNB:
    case ZYDIS_MNEMONIC_JNBE:
    case ZYDIS_MNEMONIC_JNL:
    case ZYDIS_MNEMONIC_JNLE:
    case ZYDIS_MNEMONIC_JNO:
    case ZYDIS_MNEMONIC_JNP:
    case ZYDIS_MNEMONIC_JNS:
    case ZYDIS_MNEMONIC_JNZ:
    case ZYDIS_MNEMONIC_JO:
    case ZYDIS_MNEMONIC_JP:
    case ZYDIS_MNEMONIC_JRCXZ:
    case ZYDIS_MNEMONIC_JS:
    case ZYDIS_MNEMONIC_JZ:
      return true;
    default:
      return false;
  }
}

bool CfgBuilder::IsUnconditionalJump(ZydisMnemonic mnemonic) const {
  return mnemonic == ZYDIS_MNEMONIC_JMP;
}

bool CfgBuilder::IsReturn(ZydisMnemonic mnemonic) const {
  return mnemonic == ZYDIS_MNEMONIC_RET;
}

bool CfgBuilder::IsDirectBranch(const X86Instruction& instruction) const {
  if (instruction.get_operand_count() == 0) {
    return false;
  }
  return instruction.get_operand(0).type == ZYDIS_OPERAND_TYPE_IMMEDIATE;
}

Address CfgBuilder::GetJumpTarget(const X86Instruction& instruction) const {
  const ZydisDecodedOperand& operand = instruction.get_operand(0);
  if (operand.type == ZYDIS_OPERAND_TYPE_IMMEDIATE && operand.imm.is_relative) {
    return instruction.get_address() + instruction.get_length() + operand.imm.value.s;
  }
  return operand.imm.value.u;
}

ControlFlowGraph CfgBuilder::Build() const {
  ControlFlowGraph graph;
  if (instructions_.empty()) {
    return graph;
  }

  // 1. Identify basic block leaders
  std::set<Address> leaders = {instructions_.front().get_address()};
  for (size_t i = 0; i < instructions_.size(); ++i) {
    ZydisMnemonic mnemonic = instructions_[i].get_mnemonic();
    bool is_branch = IsConditionalJump(mnemonic) || IsUnconditionalJump(mnemonic);

    if (is_branch && IsDirectBranch(instructions_[i])) {
      leaders.insert(GetJumpTarget(instructions_[i]));
    }

    if ((is_branch || IsReturn(mnemonic)) && i + 1 < instructions_.size()) {
      leaders.insert(instructions_[i + 1].get_address());
    }
  }

  // 2. Partition instructions into basic blocks
  std::map<Address, BasicBlock> blocks;
  BasicBlock current;
  for (size_t i = 0; i < instructions_.size(); ++i) {
    current.AddInstruction(instructions_[i]);
    bool is_last = (i + 1 == instructions_.size());
    bool next_is_leader = !is_last && leaders.count(instructions_[i + 1].get_address());

    if (is_last || next_is_leader) {
      blocks[current.GetStartAddress()] = current;
      current = BasicBlock();
    }
  }

  // 3. Connect control-flow edges
  for (auto& [start, block] : blocks) {
    const X86Instruction& last = block.GetInstructions().back();
    ZydisMnemonic mnemonic = last.get_mnemonic();
    Address fallthrough = last.get_address() + last.get_length();

    if (IsConditionalJump(mnemonic)) {
      if (IsDirectBranch(last)) {
        Address target = GetJumpTarget(last);
        if (blocks.count(target)) {
          block.AddSuccessor(target);
          blocks[target].AddPredecessor(start);
        }
      }
      if (blocks.count(fallthrough)) {
        block.AddSuccessor(fallthrough);
        blocks[fallthrough].AddPredecessor(start);
      }
    } else if (IsUnconditionalJump(mnemonic)) {
      if (IsDirectBranch(last)) {
        Address target = GetJumpTarget(last);
        if (blocks.count(target)) {
          block.AddSuccessor(target);
          blocks[target].AddPredecessor(start);
        }
      }
      // Never add a fall-through edge for unconditional jumps!
    } else if (!IsReturn(mnemonic)) {
      // Normal sequential execution block
      if (blocks.count(fallthrough)) {
        block.AddSuccessor(fallthrough);
        blocks[fallthrough].AddPredecessor(start);
      }
    }
  }

  for (const auto& [start, block] : blocks) {
    graph.AddBlock(block);
  }
  graph.SetEntryAddress(instructions_.front().get_address());
  return graph;
}
