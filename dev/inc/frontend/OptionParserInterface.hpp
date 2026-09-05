#ifndef DEV_INC_FRONTEND_OPTIONPARSERINTERFACE_HPP_
#define DEV_INC_FRONTEND_OPTIONPARSERINTERFACE_HPP_

#include <cstdint>
#include <string>

#define PURE_VIRTUAL_ADD_OPTION(type_)\
    virtual void AddOption(const std::string& name, type_& value, bool required, const std::string& description) = 0;  // NOLINT

namespace rosetta {
namespace frontend {

enum PipeLineStage : int {
    kLoader,
    kDecoder,
    // -- Add new stages here --
    kAll,
};

struct Configurations {
    std::string InputFile;
    std::string OutputFile;
    PipeLineStage PipelineStage;
};

class OptionParserInterface {
 public:
    virtual ~OptionParserInterface() = default;
    PURE_VIRTUAL_ADD_OPTION(int);
    PURE_VIRTUAL_ADD_OPTION(bool);
    PURE_VIRTUAL_ADD_OPTION(std::string);
    virtual void Parse(int argc, char* argv[]) = 0;
    virtual void PrintHelp() = 0;
    virtual Configurations GetConfigurations() = 0;
};

}  // namespace frontend
}  // namespace rosetta

#endif  // DEV_INC_FRONTEND_OPTIONPARSERINTERFACE_HPP_
