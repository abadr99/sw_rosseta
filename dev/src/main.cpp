#include <iostream>

#include "Engine/RosettaTranslationEngine.hpp"

int main(int argc, char* argv[]) {
    RosettaTranslationEngine engine;
    return engine.Run(argc, argv);
}
