#include "Engine/RosettaTranslationEngine.hpp"

#include <iostream>
#include <memory>
#include <string>

#include "frontend/BinaryLoaderInterface.hpp"
#include "frontend/LiefBinaryLoader.hpp"
#include "frontend/Decoder.hpp"
#include "frontend/Cli11OptionParser.hpp"

namespace FrontEnd = rosetta::frontend;
using OptionParser = rosetta::frontend::Cli11OptionParser;
using rosetta::frontend::loader::LiefBinaryParser;
using rosetta::frontend::loader::Architecture;
using rosetta::frontend::decode::Decoder;

int RosettaTranslationEngine::Run(int argc, char* argv[]) {
    
    // ---------------------------------------------------------
    // 1. ParseConfigurations Function
    // ---------------------------------------------------------
    auto ParseConfigurations = [&]() {
        std::unique_ptr<FrontEnd::OptionParserInterface> parser = std::make_unique<OptionParser>();
        std::string stage;
        
        parser->AddOption("-i,--input", cnf_.InputFile, true, "Input file");
        parser->AddOption("-o,--output", cnf_.OutputFile, true, "Output file");
        parser->AddOption("-s,--stop-after", stage, false, "Pipeline stage. stop after {loader, decoder, all}");
        parser->AddOption("-d,--dump-input-instructions", cnf_.DumpInputInstructions, false, "Dump input instructions");
        
        parser->Parse(argc, argv);
        
        if (stage.empty()) {
            cnf_.PipelineStage = FrontEnd::PipeLineStage::kAll;
        } else if (stage == "loader") {
            cnf_.PipelineStage = FrontEnd::PipeLineStage::kLoader;
        } else if (stage == "decoder") {
            cnf_.PipelineStage = FrontEnd::PipeLineStage::kDecoder;
        } else {
            parser->PrintHelp();
            exit(1); 
        }
    };

    // ---------------------------------------------------------
    // 2. RunFrontend Function
    // ---------------------------------------------------------
    auto RunFrontend = [&]() -> int {
        auto parser = std::make_unique<LiefBinaryParser>(cnf_.InputFile);
        if (parser->GetArchitecture() != Architecture::kX86_64) {
            std::cerr << "Error: Failed to parse x86-64 ELF binary.\n";
            return 1;
        }

        const auto section = parser->GetExecutableCode();
        if (section.Data.empty()) {
            std::cerr << "Error: No executable section found.\n";
            return 1;
        }

        if (cnf_.PipelineStage == FrontEnd::PipeLineStage::kLoader) {
            std::cout << "[Loader] Successfully loaded executable section at 0x"
                      << std::hex << section.VirtualAddress << std::dec
                      << " (" << section.Data.size() << " bytes)\n";
            return 0; 
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
                std::cerr << "Error: Unable to decode instruction at VMA 0x"
                          << std::hex << vma << "\n";
                return 1;
            }

            if (cnf_.DumpInputInstructions) {
                std::cout << instruction->get_text() << "\n";
            }

            const auto length = instruction->get_length();
            offset += length;
            vma += length;
        }

        return 0;
    };

    // ---------------------------------------------------------
    // 3. Optimize Function
    // ---------------------------------------------------------
    auto Optimize = [&]() -> int {
        // TODO: Implement optimization passes
        return 0;
    };

    // ---------------------------------------------------------
    // 4. Generate Function
    // ---------------------------------------------------------
    auto Generate = [&]() -> int {
        // TODO: Implement code generation
        return 0;
    };

    // =========================================================
    // Execution Flow (Using the 4 functions defined above)
    // =========================================================
    
    ParseConfigurations();

    int status = RunFrontend();
    if (status != 0) return status;

    // Check if we should stop early based on the parsed config
    if (cnf_.PipelineStage == FrontEnd::PipeLineStage::kLoader ||
        cnf_.PipelineStage == FrontEnd::PipeLineStage::kDecoder) {
        return 0; 
    }

    status = Optimize();
    if (status != 0) return status;

    return Generate();
}
