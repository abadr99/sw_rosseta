#include "Decoder.hpp"
#include <iostream>
#include <vector>
#include <iomanip>

int main() {
    std::cout << "--- sw_rosetta: 64-bit x86 to RISC-V Translator ---\n\n";

    Decoder decoder;

    // Authentic 64-bit instructions:
    std::vector<uint8_t> x86_64_binary = {
        0x48, 0x01, 0xC8,                         // add rax, rcx
        0x48, 0xC7, 0xC3, 0x78, 0x56, 0x34, 0x12, // mov rbx, 0x12345678
        0x8B, 0x44, 0x8B, 0x10                    // mov eax, [rbx + rcx*4 + 0x10]
    };

    // We start executing at an arbitrary virtual memory address
    uint64_t vma = 0x400000; 
    size_t pc = 0;

    while (pc < x86_64_binary.size()) {
        // Use the new std::optional decode method!
        auto result = decoder.decode(vma, &x86_64_binary[pc], x86_64_binary.size() - pc);

        if (!result.has_value()) {
            std::cerr << "Hardware Fault: Invalid opcode detected at VMA 0x" 
                      << std::hex << vma << "\n";
            break; 
        }

        X86Instruction ir = result.value();

        // Print the beautifully formatted assembly string provided by ZydisFormatter
        std::cout << "0x" << std::hex << ir.address << std::dec << ": " << ir.text << "\n";
        
        // Print the metadata
        std::cout << "  -> Length:   " << (int)ir.length << " bytes\n";
        std::cout << "  -> Category: " << ir.get_category_string() << "\n";
        std::cout << "  -> Flags Read: " << ir.get_flags_string(ir.flags_read) 
                  << " | Written: " << ir.get_flags_string(ir.flags_written) << "\n";
        
        std::cout << "------------------------------------------\n";
        
        pc += ir.length;
        vma += ir.length;
    }

    return 0;
}