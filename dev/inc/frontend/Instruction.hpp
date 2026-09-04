#pragma once

#include <Zydis/Zydis.h>
#include <cstdint>
#include <stdexcept>
#include <string>

namespace rosetta {
namespace frontend {
namespace decode {

class X86Instruction {
 public:
  X86Instruction();

  X86Instruction(uint64_t addr, const ZydisDecodedInstruction& z_inst,
                 const ZydisDecodedOperand* z_ops, std::string disassembly);

  // Getters for the backend translator
  uint64_t get_address() const { return address; }
  ZydisMnemonic get_mnemonic() const { return mnemonic; }
  uint8_t get_length() const { return length; }
  std::string get_text() const { return text; }

  ZydisAccessedFlagsMask get_flags_read() const { return flags_read; }
  ZydisAccessedFlagsMask get_flags_written() const { return flags_written; }

  uint8_t get_operand_count() const { return operand_count; }
  const ZydisDecodedOperand& get_operand(uint8_t index) const;

 private:
  // Core Identity
  uint64_t address;
  ZydisMnemonic mnemonic;
  uint8_t length;
  std::string text;

  // CPU Flags
  ZydisAccessedFlagsMask flags_read;
  ZydisAccessedFlagsMask flags_written;

  // Source and Destination Operands
  uint8_t operand_count;
  ZydisDecodedOperand operands[ZYDIS_MAX_OPERAND_COUNT];
};

}  // namespace decode
}  // namespace frontend
}  // namespace rosetta
