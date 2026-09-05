#include <string>
#include <vector>
#include <iostream>

#include "frontend/Cli11OptionParser.hpp"
#include "frontend/OptionParserInterface.hpp"

using rosetta::frontend::Cli11OptionParser;
using rosetta::frontend::Configurations;

Cli11OptionParser::Cli11OptionParser()
: app_("Rosetta")
{ /* EMPTY*/}

void Cli11OptionParser::AddOption(const std::string& name,
                                  int& value,
                                  bool required,
                                  const std::string& description) {
    app_.add_option(name, value, description)->required(required);
}

void Cli11OptionParser::AddOption(const std::string& name,
                                  bool& value,
                                  bool required,
                                  const std::string& description) {
    app_.add_option(name, value, description)->required(required);
}

void Cli11OptionParser::AddOption(const std::string& name,
                                  std::string& value,
                                  bool required,
                                  const std::string& description) {
    app_.add_option(name, value, description)->required(required);
}

void Cli11OptionParser::Parse(int argc, char* argv[]) {
    std::vector<std::string> v;
    for (uint32_t i = 1 ; i < argc; ++i) {
        v.push_back(argv[i]);
    }
    // CLI11 is expecting the vector in reverse order
    std::reverse(v.begin(), v.end());
    app_.parse(v);
}

Configurations Cli11OptionParser::GetConfigurations() {
    return configurations_;
}

void Cli11OptionParser::PrintHelp() {
    std::cout << app_.help() << std::endl;
}
