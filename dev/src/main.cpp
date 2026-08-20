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
        std::cout << "  -> Category: " << ir.category << "\n";
        std::cout << "  -> Flags Read: " << ir.flags_read << " | Written: " << ir.flags_written << "\n";
        
        for (uint8_t i = 0; i < ir.operand_count; i++) {
            std::cout << "     Op " << (int)i 
                      << " Type: " << ir.operands[i].type
                      << " (Size: " << ir.operands[i].size << " bits)"; // Now prints the bit size!
            
            if (ir.is_operand_register(i)) {
                std::cout << " [Reg ID: " << ir.operands[i].reg.value << "]";
            }
            std::cout << "\n";
        }
        std::cout << "------------------------------------------\n";

        pc += ir.length;
    }

    return 0;
}