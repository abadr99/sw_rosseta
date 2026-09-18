#include "frontend/IrBasicBlock.hpp"

#include <algorithm>
#include <utility>

namespace rosetta {
namespace frontend {
namespace ir {

IrBasicBlock::IrBasicBlock(rosetta::utils::Address start_address)
    : start_address_(start_address) {}

rosetta::utils::Address IrBasicBlock::StartAddress() const {
  return start_address_;
}

void IrBasicBlock::AddInstruction(IrInstruction instruction) {
  instructions_.push_back(std::move(instruction));
}

const IrBasicBlock::Instructions& IrBasicBlock::InstructionList() const {
  return instructions_;
}

IrBasicBlock::Instructions& IrBasicBlock::InstructionList() {
  return instructions_;
}

void IrBasicBlock::AddSuccessor(rosetta::utils::Address successor) {
  if (std::find(successors_.begin(), successors_.end(), successor) == successors_.end()) {
    successors_.push_back(successor);
  }
}

void IrBasicBlock::AddPredecessor(rosetta::utils::Address predecessor) {
  if (std::find(predecessors_.begin(), predecessors_.end(), predecessor) == predecessors_.end()) {
    predecessors_.push_back(predecessor);
  }
}

const IrBasicBlock::Addresses& IrBasicBlock::Successors() const {
  return successors_;
}

const IrBasicBlock::Addresses& IrBasicBlock::Predecessors() const {
  return predecessors_;
}

}  // namespace ir
}  // namespace frontend
}  // namespace rosetta
