#pragma once

#include <optional>

#include "frontend/BinaryLoaderInterface.hpp"
#include "frontend/OptionParserInterface.hpp"

class RosettaTranslationEngine {
 public:
    RosettaTranslationEngine() = default;
    
    // The only public method. It takes the CLI arguments now.
    int Run(int argc, char* argv[]);

 private:
    void ParseConfigurations(int argc, char* argv[]);
    int RunFrontEnd();
    int Load();
    int Decode();
    int Optimize();
    int Generate();

    rosetta::frontend::Configurations cnf_;
    std::optional<rosetta::frontend::loader::BinarySection> section_;
};
