#include "engine/RosettaTranslationEngine.hpp"
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


void RosettaTranslationEngine::ParseConfigurations(
    int argc, char* argv[]) {

    std::unique_ptr<FrontEnd::OptionParserInterface> parser =
        std::make_unique<OptionParser>();

    std::string stage;

    parser->AddOption("-i,--input", cnf_.InputFile, true, "Input file");
    parser->AddOption("-o,--output", cnf_.OutputFile, true, "Output file");

    parser->AddOption(
        "-s,--stop-after",
        stage,
        false,
        "Pipeline stage. stop after {loader, decoder, all}");

    parser->AddOption(
        "-d,--dump-input-instructions",
        cnf_.DumpInputInstructions,
        false,
        "Dump input instructions");

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
}

int RosettaTranslationEngine::RunFrontEnd() {
    int status = Load();

    if (status != 0) {
        return status;
    }

    if (cnf_.PipelineStage == FrontEnd::PipeLineStage::kLoader) {
        std::cout << "[Loader] Successfully loaded executable section at 0x"
                  << std::hex << section_->VirtualAddress
                  << std::dec << " ("
                  << section_->Data.size()
                  << " bytes)\n";

        return 0;
    }

    return Decode();
}

int RosettaTranslationEngine::Load() {
    std::unique_ptr<FrontEnd::loader::IBinaryParser> parser =
        std::make_unique<LiefBinaryParser>(cnf_.InputFile);

    if (parser->GetArchitecture() != Architecture::kX86_64) {
        std::cerr << "Error: Failed to parse x86-64 ELF binary.\n";
        return 1;
    }

    section_ = parser->GetExecutableCode();

    if (section_->Data.empty()) {
        std::cerr << "Error: No executable section found.\n";
        return 1;
    }

    return 0;
}

int RosettaTranslationEngine::Decode() {
    Decoder decoder;

    uint64_t vma = section_->VirtualAddress;
    size_t offset = 0;

    while (offset < section_->Data.size()) {
        const auto instruction = decoder.decode(
            vma,
            section_->Data.data() + offset,
            section_->Data.size() - offset);

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
}

int RosettaTranslationEngine::Run(int argc, char* argv[]) {
    ParseConfigurations(argc, argv);

    int status = RunFrontEnd();
    if (status != 0) {
        return status;
    }

    if (cnf_.PipelineStage == FrontEnd::PipeLineStage::kLoader ||
        cnf_.PipelineStage == FrontEnd::PipeLineStage::kDecoder) {
        return 0;
    }

    status = Optimize();
    if (status != 0) {
        return status;
    }

    return Generate();
}

int RosettaTranslationEngine::Optimize() {
    // TODO(salah): Implement optimization passes
    return 0;
}

int RosettaTranslationEngine::Generate() {
    // TODO(salah): Implement code generation
    return 0;
}
