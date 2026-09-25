#ifndef DEV_INC_GENERATOR_CGENERATOR_H_
#define DEV_INC_GENERATOR_CGENERATOR_H_

#include <sstream>
#include <string>

namespace rosetta {
namespace generator {

class CGenerator {
 public:
    CGenerator();
    ~CGenerator();
    void GenerateIf(const std::string& condition, const std::string& then_block, const std::string& else_block);  // NOLINT
    void GenerateElse(const std::string& else_block);
    void GenerateElseIf(const std::string& condition, const std::string& then_block);
    void GenerateWhile(const std::string& condition, const std::string& body);
    void GenerateDoWhile(const std::string& condition, const std::string& body);
    void GenerateFor(const std::string& init, const std::string& condition, const std::string& increment, const std::string& body);  // NOLINT
    void GenerateSwitch(const std::string& expression, const std::string& cases);
    void GenerateDefine(const std::string& define_name, const std::string& define_value);
    void GenerateFunction(const std::string& function_name, const std::string& function_parameters);
    void GenerateVariable(const std::string& variable_name, const std::string& variable_type);
    void GenerateTypedef(const std::string& typedef_name, const std::string& typedef_type);
    void GenerateInclude(const std::string& include_name);
    void GenerateLabel(const std::string& label);
    void GenerateGoto(const std::string& label);
    void GenerateComment(const std::string& comment);
    void Emit(std::ostream& out);

 private:
    std::ostringstream code_buffer_;
};

}  // namespace generator
}  // namespace rosetta

#endif  // DEV_INC_GENERATOR_CGENERATOR_H_
