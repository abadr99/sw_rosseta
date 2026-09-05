#include <cstdint>
#include <iostream>
#include <memory>

#include "frontend/BinaryLoaderInterface.hpp"
#include "frontend/LiefBinaryLoader.hpp"
#include "frontend/Decoder.hpp"

using rosetta::frontend::loader::LiefBinaryParser;
using rosetta::frontend::loader::Architecture;
using rosetta::frontend::decode::Decoder;

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: regression_driver <binary.elf>\n";
        return 1;
    }

    auto parser = std::make_unique<LiefBinaryParser>(argv[1]);

    if (parser->GetArchitecture() != Architecture::kX86_64) {
        std::cerr << "Error: Failed to parse x86-64 ELF binary.\n";
        return 1;
    }

    const auto section = parser->GetExecutableCode();

    if (section.Data.empty()) {
        std::cerr << "Error: No executable section found.\n";
        return 1;
    }

    Decoder decoder;

    uint64_t vma = section.VirtualAddress;
    size_t offset = 0;

    while (offset < section.Data.size()) {
        const auto instruction = decoder.decode(
            vma,
            section.Data.data() + offset,
            section.Data.size() - offset);

        if (!instruction || instruction->get_length() == 0) {
            std::cerr
                << "Error: Unable to decode instruction at VMA 0x"
                << std::hex << vma << "\n";
            return 1;
        }

        std::cout
            << "0x"
            << std::hex
            << instruction->get_address()
            << std::dec
            << ": "
            << instruction->get_text()
            << "\n";

        const auto length = instruction->get_length();

        offset += length;
        vma += length;
    }

    return 0;
}
