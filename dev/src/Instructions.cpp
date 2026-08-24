#include "Instructions.hpp"

#include <string>
#include <utility>

void X86Instruction::populate(uint64_t instruction_address,
                              const ZydisDecodedInstruction& z_inst,
                              const ZydisDecodedOperand* z_ops,
                              std::string disassembly) {
    // 1. Core Identity
    name = ZydisMnemonicGetString(z_inst.mnemonic);
    text = std::move(disassembly);
    address = instruction_address;
    mnemonic = z_inst.mnemonic;
    length = z_inst.length;

    // 2. Metadata used to route to the RISC-V translation logic.
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

std::string X86Instruction::get_category_string() const {
    // Zydis has a built-in converter for Category IDs
    const char* cat_str = ZydisCategoryGetString(category);
    return cat_str ? std::string(cat_str) : "UNKNOWN";
}

std::string X86Instruction::get_flags_string(uint32_t mask) const {
    if (mask == 0) return "None";

    std::string res = "[";
    // ZYDIS_CPUFLAG_* values are masks, not bit positions.
    if (mask & ZYDIS_CPUFLAG_CF) res += " CF";  // Carry Flag
    if (mask & ZYDIS_CPUFLAG_PF) res += " PF";  // Parity Flag
    if (mask & ZYDIS_CPUFLAG_AF) res += " AF";  // Adjust Flag
    if (mask & ZYDIS_CPUFLAG_ZF) res += " ZF";  // Zero Flag
    if (mask & ZYDIS_CPUFLAG_SF) res += " SF";  // Sign Flag
    if (mask & ZYDIS_CPUFLAG_TF) res += " TF";  // Trap Flag
    if (mask & ZYDIS_CPUFLAG_IF) res += " IF";  // Interrupt Flag
    if (mask & ZYDIS_CPUFLAG_DF) res += " DF";  // Direction Flag
    if (mask & ZYDIS_CPUFLAG_OF) res += " OF";  // Overflow Flag
    res += " ]";

    return res;
}
