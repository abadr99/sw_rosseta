#include <cstdint>
#include <iostream>

#include "Decoder.hpp"
#include "binaryloader.hpp"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <binary.elf>\n";
        return 1;
    }

    const auto loader =
        rosetta::frontend::loader::BinaryLoader::create(argv[1]);

    if (!loader) {
        std::cerr << "Error: Failed to parse x86-64 ELF binary.\n";
        return 1;
    }

    const auto text = loader->get_section(".text");

    if (!text || text->get().data.empty()) {
        std::cerr << "Error: No non-empty .text section found.\n";
        return 1;
    }

    const auto& section = text->get();

    rosetta::frontend::decode::Decoder decoder;

    uint64_t vma = section.vma;
    size_t offset = 0;

    while (offset < section.data.size()) {
        const auto instruction = decoder.decode(
            vma,
            section.data.data() + offset,
            section.data.size() - offset);

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
