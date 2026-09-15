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

const uint32_t& Instruction::Opcode() const {
  return opcode_;
}

const std::vector<InstructionOperand>& Instruction::Operands() const {
  return operands_;
}

const Address& Instruction::Address() const {
  return address_;
}

const uint32_t& Instruction::Size() const {
  return size_;
}

const InstructionCategory& Instruction::Category() const {
  return category_;
}

const std::string& Instruction::Mnemonic() const {
  return mnemonic_;
}

const std::string& Instruction::AssemblyText() const {
  return assembly_text_;
}
