#pragma once

#include <Zydis/Zydis.h>
#include <cstdint>
#include <string>

class X86Instruction {
 public:
  // Core Identity
  std::string name;
  std::string text;
  uint64_t address = 0;
  ZydisMnemonic mnemonic = ZYDIS_MNEMONIC_INVALID;
  uint8_t length = 0;

  // Deep Metadata
  ZydisInstructionCategory category{};  // ZYDIS_CATEGORY_COND_BR
  ZydisISASet isa_set{};                // ZYDIS_ISA_SET_AVX
  ZydisBranchType branch_type{};        // ZYDIS_BRANCH_TYPE_SHORT

  // CPU Flags (Critical for emulator accuracy)
  ZydisAccessedFlagsMask flags_read = 0;
  ZydisAccessedFlagsMask flags_written = 0;

  // All Operands
  uint8_t operand_count = 0;
  ZydisDecodedOperand operands[ZYDIS_MAX_OPERAND_COUNT]{};

  // Default constructor creates an empty instruction.
  X86Instruction() = default;

  // Populates the class with its x86 address and Intel-syntax text.
  void populate(uint64_t address, const ZydisDecodedInstruction& z_inst,
                const ZydisDecodedOperand* z_ops, std::string disassembly);

  // Safety helper for the backend.
  bool is_operand_register(uint8_t index) const;

  std::string get_category_string() const;
  std::string get_flags_string(uint32_t mask) const;
};
