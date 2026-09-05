#include <iostream>
#include <memory>

#include "frontend/OptionParserInterface.hpp"
#include "frontend/Cli11OptionParser.hpp"

namespace FrontEnd = rosetta::frontend;
using OptionParser = rosetta::frontend::Cli11OptionParser;

namespace {
    void GetConfigurations(int argc, char* argv[], FrontEnd::Configurations& cnf) {
        std::unique_ptr<FrontEnd::OptionParserInterface> parser = std::make_unique<OptionParser>();
        std::string stage;
        parser->AddOption("-i,--input", cnf.InputFile, true, "Input file");
        parser->AddOption("-o,--output", cnf.OutputFile, true, "Output file");
        parser->AddOption("-s,--stop-after", stage, false, "Pipeline stage. stop after {loader, decoder, all}");  // NOLINT
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
}
int main(int argc, char* argv[]) {
    // -- Set Command line Options
    FrontEnd::Configurations cnf;
    GetConfigurations(argc, argv, cnf);
    return 0;
}
