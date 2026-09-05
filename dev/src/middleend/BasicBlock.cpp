#include <middleend/BasicBlock.hpp>

using rosetta::middleend::basicblock::BasicBlock;
using rosetta::frontend::decode::X86Instruction;
using rosetta::frontend::utils::Address;

// NOTE: BasicBlock has no bulk constructor. The caller (e.g. CfgBuilder) is
// responsible for calling AddInstruction() once per instruction, in address
// order, to populate the block.
void BasicBlock::AddInstruction(const X86Instruction& instruction) {
  if (instructions_.empty()) {
    start_address_ = instruction.get_address();
  }
  end_address_ = instruction.get_address() + instruction.get_length();
  instructions_.push_back(instruction);
}

bool BasicBlock::IsEmpty() const {
  return instructions_.empty();
}

Address BasicBlock::GetStartAddress() const {
  return start_address_;
}

Address BasicBlock::GetEndAddress() const {
  return end_address_;
}

const BasicBlock::Instructions& BasicBlock::GetInstructions() const {
  return instructions_;
}

void BasicBlock::AddSuccessor(Address successor_address) {
  successors_.push_back(successor_address);
}

void BasicBlock::AddPredecessor(Address predecessor_address) {
  predecessors_.push_back(predecessor_address);
}

const BasicBlock::Addresses& BasicBlock::GetSuccessors() const {
  return successors_;
}

const BasicBlock::Addresses& BasicBlock::GetPredecessors() const {
  return predecessors_;
}
