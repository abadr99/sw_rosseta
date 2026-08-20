#include "Decoder.hpp"
#include <iostream>
#include <vector>

int main() {
    std::cout << "--- sw_rosetta: 64-bit x86 to RISC-V Translator ---\n\n";

    Decoder decoder;

    // Authentic 64-bit instructions:
    // 1. add rax, rcx          (0x48, 0x01, 0xC8) -> 48 is the REX.W prefix for 64-bit
    // 2. mov rbx, 0x12345678   (0x48, 0xC7, 0xC3, 0x78, 0x56, 0x34, 0x12)
    std::vector<uint8_t> x86_64_binary = {
        0x48, 0x01, 0xC8, 
        0x48, 0xC7, 0xC3, 0x78, 0x56, 0x34, 0x12
    };

    size_t pc = 0;
    X86Instruction ir; // Create an empty instruction container

    while (pc < x86_64_binary.size()) {
        if (!decoder.decode_instruction(&x86_64_binary[pc], x86_64_binary.size() - pc, ir)) {
            std::cerr << "Hardware Fault: Invalid opcode detected at PC " << pc << "\n";
            break; 
        }

        std::cout << "Decoded: " << ir.name << " (Length: " << (int)ir.length << " bytes)\n";
        
        // Print the human-readable category (e.g., BINARY instead of 15)
        std::cout << "  -> Category: " << ir.get_category_string() << "\n";
        
        // Print the exact CPU flags modified
        std::cout << "  -> Flags Read: " << ir.get_flags_string(ir.flags_read) 
                  << " | Written: " << ir.get_flags_string(ir.flags_written) << "\n";
        
        for (uint8_t i = 0; i < ir.operand_count; i++) {
            std::cout << "     Op " << (int)i 
                      << " Type: " << ir.get_operand_type_string(i) 
                      << " (Size: " << ir.operands[i].size << " bits)";
            
            if (ir.is_operand_register(i)) {
                // Get the literal register name (e.g., "rax", "rbx")
                const char* reg_name = ZydisRegisterGetString(ir.operands[i].reg.value);
                std::cout << " [Reg: " << reg_name << "]";
            } 
            else if (ir.operands[i].type == ZYDIS_OPERAND_TYPE_IMMEDIATE) {
                // Extract and format the raw hexadecimal value
                std::cout << " [Value: 0x" << std::hex << ir.operands[i].imm.value.u << std::dec << "]";
            }
            std::cout << "\n";
        }
        std::cout << "------------------------------------------\n";
        
        pc += ir.length;
    }

    return 0;
}