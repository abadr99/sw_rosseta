#include <iostream>
#include <exception>

#include "Engine/RosettaTranslationEngine.hpp"

int main(int argc, char* argv[]) {
    try {
        // 1. Instantiate the engine
        RosettaTranslationEngine engine;
        
        // 2. Pass the command-line arguments and run the pipeline
        return engine.Run(argc, argv);

    } catch (const std::exception& e) {
        // Catch standard C++ exceptions (e.g., out of memory, bad memory access)
        std::cerr << "Fatal Error: " << e.what() << "\n";
        return 1;
    } catch (...) {
        // Catch anything else that gets thrown
        std::cerr << "Fatal Error: Unknown exception caught.\n";
        return 1;
    }
}