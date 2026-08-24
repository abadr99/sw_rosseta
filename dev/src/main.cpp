#include "Decoder.hpp"
#include <iostream>
#include <vector>
#include <iomanip>
#include <fstream>
#include <string>

// Helper function to read hex strings from a text file into binary bytes
std::vector<uint8_t> load_hex_file(const std::string& filename) {
    std::vector<uint8_t> binary;
    std::ifstream file(filename);
    
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file '" << filename << "'\n";
        return binary;
    }

    std::string hex_str;
    while (file >> hex_str) {
        uint8_t byte = static_cast<uint8_t>(std::stoul(hex_str, nullptr, 16));
        binary.push_back(byte);
    }
    
    return binary;
}

// NEW: Accept command-line arguments
int main(int argc, char* argv[]) {
    std::cout << "--- sw_rosetta: 64-bit x86 to RISC-V Translator ---\n\n";

    // If the user didn't provide a file, show them how to use the program and exit
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <path_to_hex_file.txt>\n";
        return 1;
    }

    // Grab the file path from the terminal arguments
    std::string filename = argv[1];
    
    // Load the binary data dynamically
    std::vector<uint8_t> x86_64_binary = load_hex_file(filename);

    if (x86_64_binary.empty()) {
        std::cerr << "Hardware Fault: Memory buffer is empty or file not found.\n";
        return 1;
    }

    Decoder decoder;
    uint64_t vma = 0x400000; 
    size_t pc = 0;

    while (pc < x86_64_binary.size()) {
        auto result = decoder.decode(vma, &x86_64_binary[pc], x86_64_binary.size() - pc);

        if (!result.has_value()) {
            std::cerr << "Hardware Fault: Invalid opcode detected at VMA 0x" 
                      << std::hex << vma << "\n";
            break; 
        }

        X86Instruction ir = result.value();

        std::cout << "0x" << std::hex << ir.address << std::dec << ": " << ir.text << "\n";
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