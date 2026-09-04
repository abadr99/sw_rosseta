#include "frontend/Instruction.hpp"

#include <stdexcept>
#include <string>
#include <utility>

using rosetta::frontend::decode::X86Instruction;

X86Instruction::X86Instruction()
    : address(0),
      mnemonic(ZYDIS_MNEMONIC_INVALID),
      length(0),
      flags_read(0),
      flags_written(0),
      operand_count(0) {
  for (int i = 0; i < ZYDIS_MAX_OPERAND_COUNT; ++i) {
    operands[i] = {};
  }
}

X86Instruction::X86Instruction(uint64_t addr,
                               const ZydisDecodedInstruction& z_inst,
                               const ZydisDecodedOperand* z_ops,
                               std::string disassembly)
    : address(addr),
      mnemonic(z_inst.mnemonic),
      length(z_inst.length),
      text(std::move(disassembly)),
      flags_read(z_inst.cpu_flags->tested),
      flags_written(z_inst.cpu_flags->modified),
      operand_count(z_inst.operand_count_visible) {
  for (uint8_t i = 0; i < operand_count; ++i) {
    operands[i] = z_ops[i];
  }
}

const ZydisDecodedOperand& X86Instruction::get_operand(uint8_t index) const {
  if (index >= operand_count) {
    throw std::out_of_range("Operand index out of bounds");
  }
  return operands[index];
}
