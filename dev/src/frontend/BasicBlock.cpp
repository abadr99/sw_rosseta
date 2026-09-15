#include "frontend/BasicBlock.hpp"

using rosetta::frontend::basicblock::BasicBlock;
using rosetta::frontend::instruction::Instruction;
using rosetta::utils::Address;

// NOTE: BasicBlock has no bulk constructor. The caller (e.g. CfgBuilder) is
// responsible for calling AddInstruction() once per instruction, in address
// order, to populate the block.
void BasicBlock::AddInstruction(const Instruction& instruction) {
  if (instructions_.empty()) {
    start_address_ = const_cast<Instruction&>(instruction).Address();
  }
  end_address_ = const_cast<Instruction&>(instruction).Address() 
                + const_cast<Instruction&>(instruction).Size();
  instructions_.push_back(instruction);
}

bool BasicBlock::IsEmpty() const {
  return instructions_.empty();
}

Address BasicBlock::StartAddress() const {
  return start_address_;
}

Address BasicBlock::EndAddress() const {
  return end_address_;
}

const BasicBlock::Instructions& BasicBlock::InstructionList() const {
  return instructions_;
}

void BasicBlock::AddSuccessor(Address successor_address) {
  successors_.push_back(successor_address);
}

void BasicBlock::AddPredecessor(Address predecessor_address) {
  predecessors_.push_back(predecessor_address);
}

const BasicBlock::Addresses& BasicBlock::Successors() const {
  return successors_;
}

const BasicBlock::Addresses& BasicBlock::Predecessors() const {
  return predecessors_;
}
