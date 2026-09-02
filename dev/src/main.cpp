#include <cstdint>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include "Decoder.hpp"
#include "binary_loader.hpp"

int main(int argc, char* argv[]) {
    std::cout << "--- sw_rosetta: 64-bit x86 to RISC-V Translator ---\n\n";

    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <binary.elf>\n";
        return 1;
    }

    // 1. Ingest the ELF file via LIEF
    auto loader_opt = loader::BinaryLoader::create(argv[1]);
    if (!loader_opt) {
        std::cerr << "Error: Failed to parse x86-64 ELF binary.\n";
        return 1;
    }

    // 2. Extract the .text section containing the executable machine code
    auto text_opt = loader_opt->get_section(".text");
    if (!text_opt) {
        std::cerr << "Error: No allocatable .text section found.\n";
        return 1;
    }

    const auto& text_section = text_opt->get();
    const std::vector<uint8_t>& x86_64_binary = text_section.data;

    if (x86_64_binary.empty()) {
        std::cerr << "Hardware Fault: .text section is empty.\n";
        return 1;
    }

    Decoder decoder;
    // 3. Initialize VMA dynamically based on the ELF layout
    uint64_t vma = text_section.vma;
    size_t pc = 0;

    // 4. Decode loop
    while (pc < x86_64_binary.size()) {
        auto result = decoder.decode(vma, &x86_64_binary[pc],
                                     x86_64_binary.size() - pc);

        if (!result.has_value()) {
            std::cerr << "Hardware Fault: Invalid opcode detected at VMA 0x"
                      << std::hex << vma << "\n";
            break;
        }

        X86Instruction ir = result.value();

        std::cout << "0x" << std::hex << ir.address << std::dec << ": "
                  << ir.text << "\n";
        std::cout << "  -> Length:   " << static_cast<int>(ir.length)
                  << " bytes\n";
        std::cout << "  -> Category: " << ir.get_category_string() << "\n";
        std::cout << "  -> Flags Read: " << ir.get_flags_string(ir.flags_read)
                  << " | Written: " << ir.get_flags_string(ir.flags_written)
                  << "\n";
        std::cout << "------------------------------------------\n";

        pc += ir.length;
        vma += ir.length;
    }

    return 0;
}
