#ifndef DEV_INC_FRONTEND_INSTRUCTION_HPP_
#define DEV_INC_FRONTEND_INSTRUCTION_HPP_

#include <string>
#include "utils/Types.hpp"

namespace rosetta {
namespace frontend {
namespace instruction {

enum InstructionCategory {
  kArithmetic,
  kLogical,
  kCondControlFlow,
  kUnCondControlFlow,
};

enum class OperandType { 
  kUnkown, 
  kRegister, 
  kImmediate, 
  kMemory 
};

struct InstructionOperand {
  OperandType Type;
  uint32_t Reg;  // Used with Type = kRegister
  uint64_t Imm;  // Used with Type = kImmediate
  struct {
    // mov rax, [rbx + rcx * 8 + 0x10]
    // Base: rbx
    // Index: rcx
    // Scale: 8
    // Offset: 0x10
    utils::Address Base;
    utils::Address Index;
    uint32_t Scale;
    uint64_t Offset;
  } Mem;  // Used with Type = kMemory
};
class Instruction {
 public:
  Instruction(uint32_t machine_opcode, 
              std::vector<InstructionOperand> operands, 
              utils::Address address, 
              uint32_t size, 
              InstructionCategory category, 
              std::string mnemonic, 
              std::string assembly_text);
  uint32_t& Opcode();
  std::vector<InstructionOperand>& Operands();
  utils::Address& Address();
  uint32_t& Size();
  InstructionCategory& Category();
  std::string& Mnemonic();
  std::string& AssemblyText();

 private:
  uint32_t opcode_;
  std::vector<InstructionOperand> operands_;
  utils::Address address_;
  uint32_t size_;
  InstructionCategory category_;
  std::string mnemonic_;
  std::string assembly_text_;
};

}  // namespace instruction
}  // namespace frontend
}  // namespace rosetta

#endif  // DEV_INC_FRONTEND_INSTRUCTION_HPP_