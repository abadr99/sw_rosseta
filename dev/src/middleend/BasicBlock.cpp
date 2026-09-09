#include <middleend/BasicBlock.hpp>
#include <algorithm>

using rosetta::middleend::basicblock::BasicBlock;
using rosetta::frontend::decode::X86Instruction;
using rosetta::frontend::utils::Address;

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
  if (std::find(successors_.begin(), successors_.end(), successor_address) == successors_.end()) {
    successors_.push_back(successor_address);
  }
}

void BasicBlock::AddPredecessor(Address predecessor_address) {
  if (std::find(predecessors_.begin(), predecessors_.end(), predecessor_address)
    == predecessors_.end()) {
    predecessors_.push_back(predecessor_address);
  }
}

const BasicBlock::Addresses& BasicBlock::GetSuccessors() const {
  return successors_;
}

const BasicBlock::Addresses& BasicBlock::GetPredecessors() const {
  return predecessors_;
}
