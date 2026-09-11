#include "frontend/Instruction.hpp"

#include <vector>
#include <string>

using rosetta::utils::Address;
using rosetta::frontend::instruction::Instruction;
using rosetta::frontend::instruction::InstructionOperand;
using rosetta::frontend::instruction::InstructionCategory;

Instruction::Instruction(uint32_t machine_opcode,
                         std::vector<InstructionOperand> operands,
                         utils::Address address,
                         uint32_t size,
                         InstructionCategory category,
                         std::string mnemonic,
                         std::string assembly_text)
: opcode_(machine_opcode)
, operands_(operands)
, address_(address)
, size_(size)
, category_(category)
, mnemonic_(mnemonic)
, assembly_text_(assembly_text)
{ /* EMPTY */ }

uint32_t& Instruction::Opcode() {
  return opcode_;
}

std::vector<InstructionOperand>& Instruction::Operands() {
  return operands_;
}

Address& Instruction::Address() {
  return address_;
}

uint32_t& Instruction::Size() {
  return size_;
}

InstructionCategory& Instruction::Category() {
  return category_;
}

std::string& Instruction::Mnemonic() {
  return mnemonic_;
}

std::string& Instruction::AssemblyText() {
  return assembly_text_;
}
