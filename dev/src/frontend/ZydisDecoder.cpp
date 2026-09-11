#include "frontend/ZydisDecoder.hpp"

#include <Zydis/Zydis.h>

#include <cstdint>
#include <memory>
#include <utility>
#include <vector>

using namespace rosetta::frontend;  // NOLINT
using namespace rosetta::frontend::decoder;  // NOLINT
using rosetta::frontend::instruction::Instruction;
using rosetta::frontend::instruction::InstructionCategory;
using rosetta::frontend::instruction::InstructionOperand;
using rosetta::frontend::instruction::OperandType;

// Changes ZydisRegister type to integer with conventional order of regs
// starting from 1 up until 16.
uint32_t ZydisInstructionDecoder::RegisterToNumber(ZydisRegister reg) const {
  const ZydisRegister widened =
      ZydisRegisterGetLargestEnclosing(ZYDIS_MACHINE_MODE_LONG_64, reg);
  switch (widened) {
    case ZYDIS_REGISTER_RAX: return 0;
    case ZYDIS_REGISTER_RBX: return 1;
    case ZYDIS_REGISTER_RCX: return 2;
    case ZYDIS_REGISTER_RDX: return 3;
    case ZYDIS_REGISTER_RSI: return 4;
    case ZYDIS_REGISTER_RDI: return 5;
    case ZYDIS_REGISTER_RBP: return 6;
    case ZYDIS_REGISTER_RSP: return 7;
    case ZYDIS_REGISTER_R8:  return 8;
    case ZYDIS_REGISTER_R9:  return 9;
    case ZYDIS_REGISTER_R10: return 10;
    case ZYDIS_REGISTER_R11: return 11;
    case ZYDIS_REGISTER_R12: return 12;
    case ZYDIS_REGISTER_R13: return 13;
    case ZYDIS_REGISTER_R14: return 14;
    case ZYDIS_REGISTER_R15: return 15;
    default: return 16;
  }
}

InstructionOperand ZydisInstructionDecoder::ToOperand(const ZydisDecodedOperand& z_op) const {
  InstructionOperand op{};

  switch (z_op.type) {
    case ZYDIS_OPERAND_TYPE_REGISTER:
      op.Type = OperandType::kRegister;
      op.Reg = RegisterToNumber(z_op.reg.value);
      break;
    case ZYDIS_OPERAND_TYPE_IMMEDIATE:
      op.Type = OperandType::kImmediate;
      op.Imm = static_cast<uint64_t>(z_op.imm.value.s);
      break;
    case ZYDIS_OPERAND_TYPE_MEMORY:
      op.Type = OperandType::kMemory;
      op.Mem.Base = static_cast<utils::Address>(RegisterToNumber(z_op.mem.base));
      op.Mem.Index = static_cast<utils::Address>(RegisterToNumber(z_op.mem.index));
      op.Mem.Scale = z_op.mem.scale;
      op.Mem.Offset = static_cast<uint64_t>(z_op.mem.disp.value);
      break;
    default:
      op.Type = OperandType::kUnkown;
      break;
  }
  return op;
}

// TODO(abdelrhmanatta): Add cases when expanding the supported ISA.
//
// Unmapped instructions currently default to kUnknown and should be 
// explicitly categorized when working with the full ISA.
InstructionCategory ZydisInstructionDecoder::ToCategory(ZydisInstructionCategory z_category) const {
  switch (z_category) {
    case ZYDIS_CATEGORY_COND_BR:
      return InstructionCategory::kCondControlFlow;

    case ZYDIS_CATEGORY_RET:
      return InstructionCategory::kUnCondControlFlow;

    case ZYDIS_CATEGORY_DATAXFER:
      return InstructionCategory::kDataTransfer;

    case ZYDIS_CATEGORY_BINARY:
      return InstructionCategory::kArithmetic;

    default:
      return InstructionCategory::kUnknown;
  }
}

std::unique_ptr<Instruction> ZydisInstructionDecoder::Decode(
    utils::Address vma, const uint8_t* buffer, utils::Size length) const {
  if (!buffer || !length) {
    return nullptr;
  }

  ZydisDecoder z_decoder;
  ZydisFormatter z_formatter;
  ZydisDecoderInit(&z_decoder, ZYDIS_MACHINE_MODE_LONG_64, ZYDIS_STACK_WIDTH_64);
  // AT&T/GCC syntax.
  ZydisFormatterInit(&z_formatter, ZYDIS_FORMATTER_STYLE_ATT);

  ZydisDecodedInstruction instruction;
  ZydisDecodedOperand z_ops[ZYDIS_MAX_OPERAND_COUNT];

  if (!ZYAN_SUCCESS(ZydisDecoderDecodeFull(&z_decoder, buffer, length,
                                           &instruction, z_ops))) {
    return nullptr;
  }

  char formatted[256];
  if (!ZYAN_SUCCESS(ZydisFormatterFormatInstruction(
          &z_formatter, &instruction, z_ops,
          instruction.operand_count_visible, formatted, sizeof(formatted),
          vma, ZYAN_NULL))) {
    return nullptr;
  }

  std::vector<InstructionOperand> operands;
  for (uint8_t i = 0; i < instruction.operand_count_visible; ++i) {
    operands.push_back(ToOperand(z_ops[i]));
  }

  const InstructionCategory category = ToCategory(instruction.meta.category);

  return std::make_unique<Instruction>(
      instruction.opcode, std::move(operands), vma, instruction.length,
      category, ZydisMnemonicGetString(instruction.mnemonic), formatted);
}
