#include "middleend/ControlFlowGraph.hpp"
#include "utils/Macros.hpp"

using rosetta::middleend::cfg::ControlFlowGraph;
using rosetta::middleend::basicblock::BasicBlock;
using rosetta::frontend::utils::Address;

void ControlFlowGraph::AddBlock(const BasicBlock& block) {
  blocks_[block.GetStartAddress()] = block;
}

void ControlFlowGraph::SetEntryAddress(Address entry_address) {
  entry_address_ = entry_address;
}

bool ControlFlowGraph::HasBlock(Address address) const {
  return blocks_.find(address) != blocks_.end();
}

const BasicBlock& ControlFlowGraph::GetBlock(Address address) const {
  auto it = blocks_.find(address);
  if (it == blocks_.end()) {
    UNREACHABLE("No basic block at given address");
  }
  return it->second;
}

BasicBlock& ControlFlowGraph::GetBlock(Address address) {
  auto it = blocks_.find(address);
  if (it == blocks_.end()) {
    UNREACHABLE("No basic block at given address");
  }
  return it->second;
}

Address ControlFlowGraph::GetEntryAddress() const {
  return entry_address_;
}

const ControlFlowGraph::Blocks& ControlFlowGraph::GetBlocks() const {
  return blocks_;
}
