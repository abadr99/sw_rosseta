#include "Instructions.hpp"

void X86Instruction::populate(const ZydisDecodedInstruction& z_inst, const ZydisDecodedOperand* z_ops) {
    // 1. Core Identity
    name = ZydisMnemonicGetString(z_inst.mnemonic);
    mnemonic = z_inst.mnemonic;
    length = z_inst.length;

    // 2. Metadata (Essential for routing to the right RISC-V translation logic)
    category = z_inst.meta.category;
    isa_set = z_inst.meta.isa_set;
    branch_type = z_inst.meta.branch_type;
    
    // 3. CPU Flag Effects (Updated for Zydis v4 API)
    if (z_inst.cpu_flags) {
        flags_read = z_inst.cpu_flags->tested;
        flags_written = z_inst.cpu_flags->modified;
    } else {
        flags_read = 0;
        flags_written = 0;
    }

    // 4. Extract EVERY visible operand
    operand_count = z_inst.operand_count_visible;
    for (uint8_t i = 0; i < operand_count; ++i) {
        operands[i] = z_ops[i];
    }
}

bool X86Instruction::is_operand_register(uint8_t index) const {
    if (index >= operand_count) return false;
    return operands[index].type == ZYDIS_OPERAND_TYPE_REGISTER;
}