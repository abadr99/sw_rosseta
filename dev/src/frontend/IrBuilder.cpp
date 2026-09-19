#include "frontend/IrBuilder.hpp"

namespace rosetta {
namespace frontend {
namespace ir {

IrBuilder::IrBuilder(cfg::ControlFlowGraph& graph)
    : graph_(graph) {}

IrOperand IrBuilder::NewVirtualReg(IrDataType data_type) {
  IrOperand operand;

  operand.Type = IrOperandType::kVirtualReg;
  operand.DataType = data_type;
  operand.Value = next_vreg_id_++;

  return operand;
}

IrOperand IrBuilder::LowerImmediateOperand(uint64_t value) const {
  IrOperand operand;

  operand.Type = IrOperandType::kConstant;

  // TODO(@Salah) : Derive the IR data type from the actual x86 operand width
  // instead of assuming every immediate is i64.
  operand.DataType = IrDataType::kI64;

  operand.Value = value;

  return operand;
}

IrOperand IrBuilder::LowerRegisterOperand(
    uint32_t machine_reg,
    basicblock::BasicBlock& block) {

  auto it = register_map_.find(machine_reg);

  if (it == register_map_.end()) {
    // TODO(@Salah): Define the policy for reading an undefined guest register.
    // Currently return an empty operand; later this may need explicit
    // undefined/unknown handling or initial guest-state values.
    return IrOperand();
  }

  return it->second;
}

void IrBuilder::LiftMove(
    const instruction::Instruction& inst,
    basicblock::BasicBlock& block) {

  const auto& operands = inst.Operands();

  if (operands.size() != 2) {
    return;
  }

  const auto& destination = operands[0];
  const auto& source = operands[1];

  if (destination.Type != instruction::OperandType::kRegister) {
    return;
  }

  const uint32_t destination_reg = destination.Reg;

  // mov immediate -> register
  if (source.Type == instruction::OperandType::kImmediate) {
    // TODO(@Salah): Derive the virtual register type from the x86 destination
    // operand width instead of always using i64.
    IrOperand result = NewVirtualReg(IrDataType::kI64);

    IrOperand constant = LowerImmediateOperand(source.Imm);

    block.AddIrInstruction(
        IrInstruction(
            IrOpcode::kConst,
            {constant},
            result,
            0,
            0,
            inst.Address()));

    register_map_[destination_reg] = result;
    return;
  }

  // mov register -> register
  if (source.Type == instruction::OperandType::kRegister) {
    IrOperand source_value =
        LowerRegisterOperand(source.Reg, block);

    if (source_value.Type == IrOperandType::kNone) {
      return;
    }

    IrOperand result =
        NewVirtualReg(source_value.DataType);

    block.AddIrInstruction(
        IrInstruction(
            IrOpcode::kMove,
            {source_value},
            result,
            0,
            0,
            inst.Address()));

    register_map_[destination_reg] = result;
  }
}

void IrBuilder::LiftSub(
    const instruction::Instruction& inst,
    basicblock::BasicBlock& block) {

  const auto& operands = inst.Operands();

  if (operands.empty()) {
    return;
  }

  if (operands.size() == 1) {
    const auto& destination = operands[0];

    if (destination.Type != instruction::OperandType::kRegister) {
      return;
    }

    IrOperand current_value =
        LowerRegisterOperand(destination.Reg, block);

    if (current_value.Type == IrOperandType::kNone) {
      // TODO(@Salah): Define policy for undefined guest-register reads.
      return;
    }

    // TODO(@Salah): Derive immediate type from actual x86 operand width.
    IrOperand one = LowerImmediateOperand(1);

    IrOperand result =
        NewVirtualReg(current_value.DataType);

    block.AddIrInstruction(
        IrInstruction(
            IrOpcode::kSub,
            {current_value, one},
            result,
            0,
            0,
            inst.Address()));

    register_map_[destination.Reg] = result;

    // TODO(@Salah): Model x86 flags affected by DEC.
    return;
  }

  if (operands.size() == 2) {
    const auto& destination = operands[0];
    const auto& source = operands[1];

    if (destination.Type != instruction::OperandType::kRegister) {
      return;
    }

    IrOperand destination_value =
        LowerRegisterOperand(destination.Reg, block);

    if (destination_value.Type == IrOperandType::kNone) {
      // TODO(@Salah): Define policy for undefined guest-register reads.
      return;
    }

    IrOperand source_value;

    if (source.Type == instruction::OperandType::kRegister) {
      source_value =
          LowerRegisterOperand(source.Reg, block);

      if (source_value.Type == IrOperandType::kNone) {
        // TODO(@Salah): Define policy for undefined guest-register reads.
        return;
      }
    } else if (source.Type == instruction::OperandType::kImmediate) {
      // TODO(@Salah): Derive immediate type from actual x86 operand width.
      source_value =
          LowerImmediateOperand(source.Imm);
    } else {
      // TODO(@Salah): Add memory-source support.
      return;
    }

    IrOperand result =
        NewVirtualReg(destination_value.DataType);

    block.AddIrInstruction(
        IrInstruction(
            IrOpcode::kSub,
            {destination_value, source_value},
            result,
            0,
            0,
            inst.Address()));

    register_map_[destination.Reg] = result;

    // TODO(@Salah): Model x86 flags affected by SUB.
  }
}

void IrBuilder::LiftMul(
    const instruction::Instruction& inst,
    basicblock::BasicBlock& block) {

  const auto& operands = inst.Operands();

  if (operands.size() != 2) {
    return;
  }

  const auto& destination = operands[0];
  const auto& source = operands[1];

  if (destination.Type != instruction::OperandType::kRegister) {
    return;
  }

  IrOperand destination_value =
      LowerRegisterOperand(destination.Reg, block);

  if (destination_value.Type == IrOperandType::kNone) {
    // TODO(@Salah): Define policy for undefined guest-register reads.
    return;
  }

  IrOperand source_value;

  if (source.Type == instruction::OperandType::kRegister) {
    source_value =
        LowerRegisterOperand(source.Reg, block);

    if (source_value.Type == IrOperandType::kNone) {
      // TODO(@Salah): Define policy for undefined guest-register reads.
      return;
    }

  } else if (source.Type == instruction::OperandType::kImmediate) {
    // TODO(@Salah): Derive immediate type from actual x86 operand width.
    source_value =
        LowerImmediateOperand(source.Imm);

  } else {
    // TODO(@Salah): Add memory-source support.
    return;
  }

  IrOperand result =
      NewVirtualReg(destination_value.DataType);

  block.AddIrInstruction(
      IrInstruction(
          IrOpcode::kMul,
          {destination_value, source_value},
          result,
          0,
          0,
          inst.Address()));

  register_map_[destination.Reg] = result;

  // TODO(@Salah): Model x86 flags affected by IMUL.
}

void IrBuilder::LiftCompareAndCondBr(
    const instruction::Instruction& cmp_inst,
    const instruction::Instruction& branch_inst,
    basicblock::BasicBlock& block) {

  const auto& cmp_operands = cmp_inst.Operands();
  const auto& branch_operands = branch_inst.Operands();

  if (cmp_operands.size() != 2 || branch_operands.size() != 1) {
    return;
  }

  const auto& lhs = cmp_operands[0];
  const auto& rhs = cmp_operands[1];

  if (lhs.Type != instruction::OperandType::kRegister) {
    return;
  }

  IrOperand lhs_value =
      LowerRegisterOperand(lhs.Reg, block);

  if (lhs_value.Type == IrOperandType::kNone) {
    // TODO(@Salah): Define policy for undefined guest-register reads.
    return;
  }

  IrOperand rhs_value;

  if (rhs.Type == instruction::OperandType::kRegister) {
    rhs_value =
        LowerRegisterOperand(rhs.Reg, block);

    if (rhs_value.Type == IrOperandType::kNone) {
      // TODO(@Salah): Define policy for undefined guest-register reads.
      return;
    }

  } else if (rhs.Type == instruction::OperandType::kImmediate) {
    // TODO(@Salah): Derive immediate type from actual x86 operand width.
    rhs_value =
        LowerImmediateOperand(rhs.Imm);

  } else {
    // TODO(@Salah): Add memory-operand support.
    return;
  }

  IrOpcode compare_opcode = IrOpcode::kNone;

  if (branch_inst.Mnemonic() == "jle") {
    compare_opcode = IrOpcode::kSle;
  } else if (branch_inst.Mnemonic() == "jg") {
    compare_opcode = IrOpcode::kSgt;
  } else {
    // TODO(@Salah): Add the remaining x86 conditional-branch predicates.
    return;
  }

  IrOperand condition =
      NewVirtualReg(IrDataType::kI1);

  block.AddIrInstruction(
      IrInstruction(
          compare_opcode,
          {lhs_value, rhs_value},
          condition,
          0,
          0,
          cmp_inst.Address()));

  if (branch_operands[0].Type != instruction::OperandType::kImmediate) {
    // TODO(@Salah): Add indirect conditional-branch support.
    return;
  }

  utils::Address true_target =
      branch_inst.Address() +
      branch_inst.Size() +
      branch_operands[0].Imm;

  utils::Address false_target =
      branch_inst.Address() +
      branch_inst.Size();

  block.AddIrInstruction(
      IrInstruction(
          IrOpcode::kCondBr,
          {condition},
          IrOperand(),
          true_target,
          false_target,
          branch_inst.Address()));

  // TODO(@Salah): Model x86 FLAGS so CMP can be represented independently
  // from the conditional branch that consumes its result.
}

void IrBuilder::LiftReturn(basicblock::BasicBlock& block) {
  const auto& instructions = block.InstructionList();

  if (instructions.empty()) {
    return;
  }

  const auto& ret_inst = instructions.back();

  IrOperand target = NewVirtualReg(IrDataType::kI64);

  block.AddIrInstruction(
      IrInstruction(
          IrOpcode::kLoadGuestStackReturnAddress,
          {},
          target,
          0,
          0,
          ret_inst.Address()));

  block.AddIrInstruction(
      IrInstruction(
          IrOpcode::kIndirectBr,
          {target},
          IrOperand(),
          0,
          0,
          ret_inst.Address()));

  // TODO(@Salah): Lower RET to explicit guest-memory load and RSP update.
}

void IrBuilder::LiftInstruction(
    const instruction::Instruction& inst,
    basicblock::BasicBlock& block) {

  switch (inst.Category()) {
    case instruction::InstructionCategory::kDataTransfer:
      LiftMove(inst, block);
      break;

    case instruction::InstructionCategory::kArithmetic:
      if (inst.Mnemonic() == "sub" || inst.Mnemonic() == "dec") {
        LiftSub(inst, block);
      } else if (inst.Mnemonic() == "imul") {
        LiftMul(inst, block);
      }
      break;

    case instruction::InstructionCategory::kUnCondControlFlow:
      if (inst.Mnemonic() == "ret") {
        LiftReturn(block);
      }
      break;

    case instruction::InstructionCategory::kCondControlFlow:
      break;

    default:
      break;
  }
}

void IrBuilder::LiftBlock(basicblock::BasicBlock& block) {
  const auto& instructions = block.InstructionList();

  for (size_t i = 0; i < instructions.size(); ++i) {
    const auto& inst = instructions[i];

    if (inst.Mnemonic() == "cmp" &&
        i + 1 < instructions.size() &&
        instructions[i + 1].Category() ==
            instruction::InstructionCategory::kCondControlFlow) {
      LiftCompareAndCondBr(inst, instructions[i + 1], block);
      ++i;
      continue;
    }

    LiftInstruction(inst, block);
  }
}

BasicBlock& ControlFlowGraph::Block(Address address) {
  auto it = blocks_.find(address);

  if (it == blocks_.end()) {
    throw std::out_of_range("Basic block not found");
  }

  return it->second;
}

void IrBuilder::Build() {
  for (const auto& [address, _] : graph_.BlocksList()) {
    LiftBlock(graph_.Block(address));
  }

  ResolvePendingPhis();
}

}  // namespace ir
}  // namespace frontend
}  // namespace rosetta
