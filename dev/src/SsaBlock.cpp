#include "frontend/SsaBlock.hpp"

namespace rosetta {
namespace frontend {
namespace ssa {

SsaBlock::SsaBlock(basicblock::BasicBlock& bb) : bb_(&bb) {}

const basicblock::BasicBlock::Instructions& SsaBlock::InstructionList() const {
  return bb_->InstructionList();
}

utils::Address SsaBlock::StartAddress() const {
  return bb_->StartAddress();
}

const basicblock::BasicBlock::Addresses& SsaBlock::Successors() const {
  return bb_->Successors();
}

const basicblock::BasicBlock::Addresses& SsaBlock::Predecessors() const {
  return bb_->Predecessors();
}

basicblock::BasicBlock& SsaBlock::Bb() {
  return *bb_;
}

void SsaBlock::AddStatement(const SsaStatement& statement) {
  statements_.push_back(statement);
}

const SsaBlock::Statements& SsaBlock::StatementList() const {
  return statements_;
}

SsaBlock::Statements& SsaBlock::StatementList() {
  return statements_;
}

}  // namespace ssa
}  // namespace frontend
}  // namespace rosetta
