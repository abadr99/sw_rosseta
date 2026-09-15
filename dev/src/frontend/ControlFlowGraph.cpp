#include "frontend/ControlFlowGraph.hpp"
#include "utils/Macros.hpp"

using rosetta::frontend::cfg::ControlFlowGraph;
using rosetta::frontend::basicblock::BasicBlock;
using rosetta::frontend::utils::Address;

// NOTE: Caller (e.g. CfgBuilder) is responsible for calling AddBlock() once
// per basic block to populate the graph, and for calling SetEntryAddress()
// once the entry block's address is known. AddBlock() keys the block under
// its own start address, so blocks are always looked up consistently.
void ControlFlowGraph::AddBlock(const BasicBlock& block) {
  blocks_[block.StartAddress()] = block;
}

void ControlFlowGraph::SetEntryAddress(Address entry_address) {
  entry_address_ = entry_address;
}

bool ControlFlowGraph::HasBlock(Address address) const {
  return blocks_.find(address) != blocks_.end();
}

const BasicBlock& ControlFlowGraph::Block(Address address) const {
  auto it = blocks_.find(address);
  if (it == blocks_.end()) {
    UNREACHABLE("No basic block at given address");
  }
  return it->second;
}

Address ControlFlowGraph::EntryAddress() const {
  return entry_address_;
}

const ControlFlowGraph::Blocks& ControlFlowGraph::BlocksList() const {
  return blocks_;
}
