#include "frontend/ControlFlowGraph.hpp"

#include "utils/Macros.hpp"
#include "utils/Types.hpp"

using rosetta::frontend::cfg::ControlFlowGraph;
using rosetta::frontend::basicblock::BasicBlock;
using rosetta::utils::Address;

void ControlFlowGraph::AddBlock(const BasicBlock& block) {
  blocks_[block.StartAddress()] = block;
  adjacency_.emplace(
      block.StartAddress(),
      AdjacencyList::mapped_type{});
}

void ControlFlowGraph::AddEdge(Address from, Address to) {
  auto it = adjacency_.find(from);

  if (it == adjacency_.end()) {
    UNREACHABLE("AddEdge: Source block not found");
  }

  it->second.push_back(to);
}

void ControlFlowGraph::SetEntryAddress(Address entry_address) {
  entry_address_ = entry_address;
}

bool ControlFlowGraph::HasBlock(Address address) const {
  return blocks_.find(address) != blocks_.end();
}

BasicBlock& ControlFlowGraph::Block(Address address) {
  auto it = blocks_.find(address);

  if (it == blocks_.end()) {
    UNREACHABLE("No basic block at given address");
  }

  return it->second;
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

const ControlFlowGraph::AdjacencyList&
ControlFlowGraph::BlocksList() const {
  return adjacency_;
}
