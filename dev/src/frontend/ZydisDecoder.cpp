#include "frontend/ZydisDecoder.hpp"

#include <vector>
#include <memory>
#include <utility>

using namespace rosetta::frontend;  // NOLINT
using namespace rosetta::frontend::decode;  // NOLINT

ZydisInstructionDecoder::ZydisInstructionDecoder() {
  ZydisDecoderInit(
    &z_decoder_, ZYDIS_MACHINE_MODE_LONG_64, ZYDIS_STACK_WIDTH_64);
  ZydisFormatterInit(&z_formatter_, ZYDIS_FORMATTER_STYLE_INTEL);
}

Operand ZydisInstructionDecoder::ToOperand(const ZydisDecodedOperand& z_op) {
  Operand op;
  op.size_bits = z_op.size;

  switch (z_op.type) {
    case ZYDIS_OPERAND_TYPE_REGISTER:
      op.kind = OperandKind::kRegister;
      op.reg = ZydisRegisterGetString(z_op.reg.value);
      break;
    case ZYDIS_OPERAND_TYPE_IMMEDIATE:
      op.kind = OperandKind::kImmediate;
      op.immediate = z_op.imm.value.s;
      break;
    // effective_address = base + (index * scale) + displacement
    case ZYDIS_OPERAND_TYPE_MEMORY:
      op.kind = OperandKind::kMemory;
      if (z_op.mem.base != ZYDIS_REGISTER_NONE) {
        op.reg = ZydisRegisterGetString(z_op.mem.base);
      }
      if (z_op.mem.index != ZYDIS_REGISTER_NONE) {
        op.index = ZydisRegisterGetString(z_op.mem.index);
      }
      op.scale = z_op.mem.scale;
      op.displacement = z_op.mem.disp.value;
      break;
    default:
      op.kind = OperandKind::kUnused;
      break;
  }
  return  op;
}

std::unique_ptr<Instruction> ZydisInstructionDecoder::Decode(
  utils::Address vma, const uint8_t* buffer, utils::Size length) const {
  if (!buffer || !length) {
    return nullptr;
  }

  ZydisDecodedInstruction instruction;
  ZydisDecodedOperand z_op[ZYDIS_MAX_OPERAND_COUNT];

  if (!ZYAN_SUCCESS(ZydisDecoderDecodeFull(
      &z_decoder_, buffer, length, &instruction, z_op))) {
    return nullptr;
  }

  char formatted[256];
  if (!ZYAN_SUCCESS(ZydisFormatterFormatInstruction(
          &z_formatter_, &instruction, z_op,
          instruction.operand_count_visible, formatted, sizeof(formatted),
          vma, ZYAN_NULL))) {
    return nullptr;
  }

  std::vector<Operand> operands;
  for (uint8_t i = 0; i < instruction.operand_count_visible; ++i) {
    operands.push_back(ToOperand(z_op[i]));
  }

  return std::make_unique<Instruction>(
      vma, ZydisMnemonicGetString(instruction.mnemonic), instruction.length,
      formatted, instruction.cpu_flags->tested, instruction.cpu_flags->modified,
      std::move(operands));
}
