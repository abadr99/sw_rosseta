#pragma once

#include "frontend/OptionParserInterface.hpp"

class RosettaTranslationEngine {
 public:
    RosettaTranslationEngine() = default;
    
    // The only public method. It takes the CLI arguments now.
    int Run(int argc, char* argv[]);

 private:
    // The ONLY private member
    rosetta::frontend::Configurations cnf_;
};
