#include <Zydis/Zydis.h>

#include <cstdint>
#include <memory>
#include <utility>
#include <vector>

#include "frontend/ZydisDecoder.hpp"
#include "utils/Macros.hpp"

using namespace rosetta::frontend;  // NOLINT
using namespace rosetta::frontend::decoder;  // NOLINT
using rosetta::frontend::instruction::Instruction;
using rosetta::frontend::instruction::InstructionCategory;
using rosetta::frontend::instruction::InstructionOperand;
using rosetta::frontend::instruction::OperandType;

// Changes ZydisRegister type to integer with conventional order of regs
// starting from 1 up until 16.
// TODO(@abdelrhmanatta): What will happen if we need to use 8/16/32 regs (eg. eax)
uint32_t ZydisInstructionDecoder::RegisterToNumber(ZydisRegister reg) const {
  const ZydisRegister widened =
      ZydisRegisterGetLargestEnclosing(ZYDIS_MACHINE_MODE_LONG_64, reg);

  if (widened >= ZYDIS_REGISTER_RAX && widened <= ZYDIS_REGISTER_R15) {
    return static_cast<uint32_t>(widened - ZYDIS_REGISTER_RAX);
  }

  return 16;
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
      UNREACHABLE("TYPE NOT SUPPORTED");
      break;
  }
  return op;
}

// TODO(abdelrhmanatta): Add cases when expanding the supported ISA.
//
// Unmapped instructions currently default to kUnknown and should be 
// explicitly categorized when working with the full ISA.
InstructionCategory ZydisInstructionDecoder::ToCategory(
    ZydisInstructionCategory z_category) const {
  switch (z_category) {
    case ZYDIS_CATEGORY_COND_BR:
      return InstructionCategory::kCondControlFlow;

    case ZYDIS_CATEGORY_UNCOND_BR:
      return InstructionCategory::kUnCondControlFlow;

    case ZYDIS_CATEGORY_CALL:
      return InstructionCategory::kCall;

    case ZYDIS_CATEGORY_RET:
      return InstructionCategory::kReturn;

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

std::vector<Instruction> ZydisInstructionDecoder::DecodeAll(
    utils::Address vma, const uint8_t* buffer, utils::Size length) const {
  std::vector<Instruction> instructions;

  if (!buffer || !length) {
    return instructions;
  }

  utils::Size offset = 0;
  while (offset < length) {
    auto instruction = Decode(vma + offset, buffer + offset, length - offset);
    if (!instruction || instruction->Size() == 0) {
      break;
    }

    const uint32_t inst_size = instruction->Size();
    instructions.push_back(std::move(*instruction));
    offset += inst_size;
  }

  return instructions;
}
