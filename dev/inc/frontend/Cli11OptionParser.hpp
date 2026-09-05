#ifndef FRONTEND_CLI11_OPTION_PARSER_HPP
#define FRONTEND_CLI11_OPTION_PARSER_HPP

#include "OptionParserInterface.hpp"
#include "cli11/CLI11.hpp"  

namespace rosetta {
namespace frontend {

class Cli11OptionParser final : public OptionParserInterface {
 public:
    ~Cli11OptionParser() = default;
    Cli11OptionParser();
    void AddOption(const std::string& name, int& value, bool required, const std::string& description) override;
    void AddOption(const std::string& name, bool& value, bool required, const std::string& description) override;
    void AddOption(const std::string& name, std::string& value, bool required, const std::string& description) override;
    void Parse(int argc, char* argv[]) override;
    void PrintHelp() override;
    Configurations GetConfigurations() override;
 private:
    CLI::App app_;
    Configurations configurations_;
};
} // namespace frontend
} // namespace rosetta

#endif // FRONTEND_CLI11_OPTION_PARSER_HPP