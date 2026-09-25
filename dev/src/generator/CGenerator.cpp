#include "generator/CGenerator.h"

CGenerator::CGenerator() {
}

CGenerator::~CGenerator() {
}

void CGenerator::GenerateIf(const std::string& condition, const std::string& then_block, const std::string& else_block) {
    code_buffer_ << "if (" << condition << ") {\n" << then_block << "}\n";
}

void CGenerator::GenerateElse(const std::string& else_block) {
    code_buffer_ << "else {\n" << else_block << "}\n";
}

void CGenerator::GenerateElseIf(const std::string& condition, const std::string& then_block) {
    code_buffer_ << "else if (" << condition << ") {\n" << then_block << "}\n";
}

void CGenerator::GenerateWhile(const std::string& condition, const std::string& body) {
    code_buffer_ << "while (" << condition << ") {\n" << body << "}\n";
}

void CGenerator::GenerateDoWhile(const std::string& condition, const std::string& body) {
    code_buffer_ << "do {\n" << body << "} while (" << condition << ")\n";
}

void CGenerator::GenerateFor(const std::string& init, const std::string& condition, const std::string& increment, const std::string& body) {
    code_buffer_ << "for (" << init << "; " << condition << "; " << increment << ") {\n" << body << "}\n";
}

void CGenerator::GenerateSwitch(const std::string& expression, const std::string& cases) {
    code_buffer_ << "switch (" << expression << ") {\n" << cases << "}\n";
}

void CGenerator::GenerateDefine(const std::string& define_name, const std::string& define_value) {
    code_buffer_ << "#define " << define_name << " " << define_value << "\n";
}

void CGenerator::GenerateFunction(const std::string& function_name, const std::string& function_parameters) {
    code_buffer_ << "void " << function_name << "(" << function_parameters << ") {\n";
}

void CGenerator::GenerateVariable(const std::string& variable_name, const std::string& variable_type) {
    code_buffer_ << variable_type << " " << variable_name << ";\n";
}

void CGenerator::GenerateTypedef(const std::string& typedef_name, const std::string& typedef_type) {
    code_buffer_ << "typedef " << typedef_type << " " << typedef_name << ";\n";
}

void CGenerator::GenerateInclude(const std::string& include_name) {
    code_buffer_ << "#include <" << include_name << ">\n";
}

void CGenerator::GenerateLabel(const std::string& label) {
    code_buffer_ << label << ":\n";
}

void CGenerator::GenerateGoto(const std::string& label) {
    code_buffer_ << "goto " << label << ";\n";
}

void CGenerator::GenerateComment(const std::string& comment) {
    code_buffer_ << "// " << comment << "\n";
}

void CGenerator::Emit(std::ostream& out) {
    out << code_buffer_.str();
}