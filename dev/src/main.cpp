#include <iostream>
#include <memory>
#include <string>

#include "frontend/BinaryLoaderInterface.hpp"
#include "frontend/LiefBinaryLoader.hpp"
#include "frontend/Decoder.hpp"
#include "frontend/OptionParserInterface.hpp"
#include "frontend/Cli11OptionParser.hpp"

namespace FrontEnd = rosetta::frontend;
using OptionParser = rosetta::frontend::Cli11OptionParser;
using rosetta::frontend::loader::LiefBinaryParser;
using rosetta::frontend::loader::Architecture;
using rosetta::frontend::decode::Decoder;

namespace {
void GetConfigurations(int argc, char* argv[], FrontEnd::Configurations& cnf) {
    std::unique_ptr<FrontEnd::OptionParserInterface> parser = std::make_unique<OptionParser>();  // NOLINT
    std::string stage;
    parser->AddOption("-i,--input", cnf.InputFile, true, "Input file");
    parser->AddOption("-o,--output", cnf.OutputFile, true, "Output file");
    parser->AddOption("-s,--stop-after", stage, false, "Pipeline stage. stop after {loader, decoder, all}");  // NOLINT
    parser->AddOption("-d,--dump-input-instructions", cnf.DumpInputInstructions,
         false, "Dump input instructions");
    parser->Parse(argc, argv);
    // Now map the stage string to the enum
    if (stage.empty()) {
        cnf.PipelineStage = FrontEnd::PipeLineStage::kAll;
    } else if (stage == "loader") {
        cnf.PipelineStage = FrontEnd::PipeLineStage::kLoader;
    } else if (stage == "decoder") {
        cnf.PipelineStage = FrontEnd::PipeLineStage::kDecoder;
    } else {
        parser->PrintHelp();
    }
}
}  // namespace

int main(int argc, char* argv[]) {
    // -- Set Command line Options
    FrontEnd::Configurations cnf;
    GetConfigurations(argc, argv, cnf);

    // 1. Stage: Loader
    auto parser = std::make_unique<LiefBinaryParser>(cnf.InputFile);
    if (parser->GetArchitecture() != Architecture::kX86_64) {
        std::cerr << "Error: Failed to parse x86-64 ELF binary.\n";
        return 1;
    }

    const auto section = parser->GetExecutableCode();
    if (section.Data.empty()) {
        std::cerr << "Error: No executable section found.\n";
        return 1;
    }

    if (cnf.PipelineStage == FrontEnd::PipeLineStage::kLoader) {
        std::cout << "[Loader] Successfully loaded executable section at 0x"
                  << std::hex << section.VirtualAddress << std::dec
                  << " (" << section.Data.size() << " bytes)\n";
        return 0;
    }

    // 2. Stage: Decoder
    Decoder decoder;
    uint64_t vma = section.VirtualAddress;
    size_t offset = 0;

    while (offset < section.Data.size()) {
        const auto instruction = decoder.decode(
            vma,
            section.Data.data() + offset,
            section.Data.size() - offset);

        if (!instruction || instruction->get_length() == 0) {
            std::cerr << "Error: Unable to decode instruction at VMA 0x"
                      << std::hex << vma << "\n";
            return 1;
        }

        if (cnf.DumpInputInstructions) {
            std::cout << instruction->get_text() << "\n";
        }

        const auto length = instruction->get_length();
        offset += length;
        vma += length;
    }

    if (cnf.PipelineStage == FrontEnd::PipeLineStage::kDecoder) {
        return 0;
    }

    return 0;
}
