#pragma once

#include <string>
#include <Zydis/Zydis.h>

class X86Instruction {
public:
    // Core Identity
    std::string name;
    ZydisMnemonic mnemonic;
    uint8_t length;
    
    // Deep Metadata
    ZydisInstructionCategory category; // e.g., ZYDIS_CATEGORY_COND_BR (Branch)
    ZydisISASet isa_set;               // e.g., ZYDIS_ISA_SET_AVX
    ZydisBranchType branch_type;       // e.g., ZYDIS_BRANCH_TYPE_SHORT
    
    // CPU Flags (Critical for emulator accuracy)
    ZydisAccessedFlagsMask flags_read;
    ZydisAccessedFlagsMask flags_written;

    // All Operands
    uint8_t operand_count;
    ZydisDecodedOperand operands[ZYDIS_MAX_OPERAND_COUNT]; // Array holding all operands

    // Default constructor (required since we pass by reference now)
    X86Instruction() = default;

    // Populates the class with everything Zydis knows
    void populate(const ZydisDecodedInstruction& z_inst, const ZydisDecodedOperand* z_ops);

    // Safety helper for the backend
    bool is_operand_register(uint8_t index) const;
};