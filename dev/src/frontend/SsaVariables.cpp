#include "frontend/SsaVariables.hpp"

namespace rosetta {
namespace frontend {
namespace ssa {

SsaVariables::SsaVariables(cfg::ControlFlowGraph& graph)
    : graph_(graph) {}

SsaBlock& SsaVariables::GetOrCreateBlock(utils::Address address) {
  auto it = ssa_blocks_.find(address);
  if (it == ssa_blocks_.end()) {
    it = ssa_blocks_.emplace(address, SsaBlock(graph_.Block(address))).first;
  }
  return it->second;
}

SsaOperand SsaVariables::NewVirtualReg(SsaDataType data_type) {
  SsaOperand operand;

  operand.Type = SsaOperandType::kVirtualReg;
  operand.DataType = data_type;
  operand.Value = next_vreg_id_++;

  return operand;
}

SsaOperand SsaVariables::LowerImmediateOperand(uint64_t value) const {
  SsaOperand operand;

  operand.Type = SsaOperandType::kConstant;

  // TODO(@Salah) : Derive the IR data type from the actual x86 operand width
  // instead of assuming every immediate is i64.
  operand.DataType = SsaDataType::kI64;

  operand.Value = value;

  return operand;
}

SsaOperand SsaVariables::LowerRegisterOperand(
    uint32_t machine_reg,
    SsaBlock& block) {

  auto it = register_map_.find(machine_reg);

  if (it == register_map_.end()) {
    // TODO(@Salah): Define the policy for reading an undefined guest register.
    // Currently return an empty operand; later this may need explicit
    // undefined/unknown handling or initial guest-state values.
    return SsaOperand();
  }

  return it->second;
}

void SsaVariables::LiftMove(
    const instruction::Instruction& inst,
    SsaBlock& block) {

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
    SsaOperand result = NewVirtualReg(SsaDataType::kI64);

    SsaOperand constant = LowerImmediateOperand(source.Imm);

    block.AddStatement(
        SsaStatement(
            SsaOpcode::kConst,
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
    SsaOperand source_value =
        LowerRegisterOperand(source.Reg, block);

    if (source_value.Type == SsaOperandType::kNone) {
      return;
    }

    SsaOperand result =
        NewVirtualReg(source_value.DataType);

    block.AddStatement(
        SsaStatement(
            SsaOpcode::kMove,
            {source_value},
            result,
            0,
            0,
            inst.Address()));

    register_map_[destination_reg] = result;
  }
}

void SsaVariables::LiftSub(
    const instruction::Instruction& inst,
    SsaBlock& block) {

  const auto& operands = inst.Operands();

  if (operands.empty()) {
    return;
  }

  if (operands.size() == 1) {
    const auto& destination = operands[0];

    if (destination.Type != instruction::OperandType::kRegister) {
      return;
    }

    SsaOperand current_value =
        LowerRegisterOperand(destination.Reg, block);

    if (current_value.Type == SsaOperandType::kNone) {
      // TODO(@Salah): Define policy for undefined guest-register reads.
      return;
    }

    // TODO(@Salah): Derive immediate type from actual x86 operand width.
    SsaOperand one = LowerImmediateOperand(1);

    SsaOperand result =
        NewVirtualReg(current_value.DataType);

    block.AddStatement(
        SsaStatement(
            SsaOpcode::kSub,
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

    SsaOperand destination_value =
        LowerRegisterOperand(destination.Reg, block);

    if (destination_value.Type == SsaOperandType::kNone) {
      // TODO(@Salah): Define policy for undefined guest-register reads.
      return;
    }

    SsaOperand source_value;

    if (source.Type == instruction::OperandType::kRegister) {
      source_value =
          LowerRegisterOperand(source.Reg, block);

      if (source_value.Type == SsaOperandType::kNone) {
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

    SsaOperand result =
        NewVirtualReg(destination_value.DataType);

    block.AddStatement(
        SsaStatement(
            SsaOpcode::kSub,
            {destination_value, source_value},
            result,
            0,
            0,
            inst.Address()));

    register_map_[destination.Reg] = result;

    // TODO(@Salah): Model x86 flags affected by SUB.
  }
}

void SsaVariables::LiftMul(
    const instruction::Instruction& inst,
    SsaBlock& block) {

  const auto& operands = inst.Operands();

  if (operands.size() != 2) {
    return;
  }

  const auto& destination = operands[0];
  const auto& source = operands[1];

  if (destination.Type != instruction::OperandType::kRegister) {
    return;
  }

  SsaOperand destination_value =
      LowerRegisterOperand(destination.Reg, block);

  if (destination_value.Type == SsaOperandType::kNone) {
    // TODO(@Salah): Define policy for undefined guest-register reads.
    return;
  }

  SsaOperand source_value;

  if (source.Type == instruction::OperandType::kRegister) {
    source_value =
        LowerRegisterOperand(source.Reg, block);

    if (source_value.Type == SsaOperandType::kNone) {
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

  SsaOperand result =
      NewVirtualReg(destination_value.DataType);

  block.AddStatement(
      SsaStatement(
          SsaOpcode::kMul,
          {destination_value, source_value},
          result,
          0,
          0,
          inst.Address()));

  register_map_[destination.Reg] = result;

  // TODO(@Salah): Model x86 flags affected by IMUL.
}

void SsaVariables::LiftCompareAndCondBr(
    const instruction::Instruction& cmp_inst,
    const instruction::Instruction& branch_inst,
    SsaBlock& block) {

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

  SsaOperand lhs_value =
      LowerRegisterOperand(lhs.Reg, block);

  if (lhs_value.Type == SsaOperandType::kNone) {
    // TODO(@Salah): Define policy for undefined guest-register reads.
    return;
  }

  SsaOperand rhs_value;

  if (rhs.Type == instruction::OperandType::kRegister) {
    rhs_value =
        LowerRegisterOperand(rhs.Reg, block);

    if (rhs_value.Type == SsaOperandType::kNone) {
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

  SsaOpcode compare_opcode = SsaOpcode::kNone;

  if (branch_inst.Mnemonic() == "jle") {
    compare_opcode = SsaOpcode::kSle;
  } else if (branch_inst.Mnemonic() == "jg") {
    compare_opcode = SsaOpcode::kSgt;
  } else {
    // TODO(@Salah): Add the remaining x86 conditional-branch predicates.
    return;
  }

  SsaOperand condition =
      NewVirtualReg(SsaDataType::kI1);

  block.AddStatement(
      SsaStatement(
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

  block.AddStatement(
      SsaStatement(
          SsaOpcode::kCondBr,
          {condition},
          SsaOperand(),
          true_target,
          false_target,
          branch_inst.Address()));

  // TODO(@Salah): Model x86 FLAGS so CMP can be represented independently
  // from the conditional branch that consumes its result.
}

void SsaVariables::LiftReturn(SsaBlock& block) {
  const auto& instructions = block.InstructionList();

  if (instructions.empty()) {
    return;
  }

  const auto& ret_inst = instructions.back();

  SsaOperand target = NewVirtualReg(SsaDataType::kI64);

  block.AddStatement(
      SsaStatement(
          SsaOpcode::kLoadGuestStackReturnAddress,
          {},
          target,
          0,
          0,
          ret_inst.Address()));

  block.AddStatement(
      SsaStatement(
          SsaOpcode::kIndirectBr,
          {target},
          SsaOperand(),
          0,
          0,
          ret_inst.Address()));

  // TODO(@Salah): Lower RET to explicit guest-memory load and RSP update.
}

void SsaVariables::LiftInstruction(
    const instruction::Instruction& inst,
    SsaBlock& block) {

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

void SsaVariables::LiftBlock(SsaBlock& block) {
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

void SsaVariables::Build() {
  for (const auto& [address, _] : graph_.BlocksList()) {
    LiftBlock(GetOrCreateBlock(address));
  }

  ResolvePendingPhis();
}

}  // namespace ssa
}  // namespace frontend
}  // namespace rosetta
