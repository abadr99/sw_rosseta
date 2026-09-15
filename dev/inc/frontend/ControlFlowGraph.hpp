#ifndef DEV_INC_FRONTEND_CONTROLFLOWGRAPH_HPP_
#define DEV_INC_FRONTEND_CONTROLFLOWGRAPH_HPP_

#include <list>
#include <map>

#include "frontend/BasicBlock.hpp"
#include "utils/Types.hpp"

namespace rosetta {
namespace frontend {
namespace cfg {

class ControlFlowGraph {
 public:
  using Blocks = std::map<utils::Address, frontend::basicblock::BasicBlock>;
  using AdjacencyList = std::map<utils::Address, std::list<utils::Address>>;

  ControlFlowGraph() = default;

  void AddBlock(const frontend::basicblock::BasicBlock& block);
  void AddEdge(utils::Address from, utils::Address to);
  void SetEntryAddress(utils::Address entry_address);

  bool HasBlock(utils::Address address) const;
  const frontend::basicblock::BasicBlock& Block(utils::Address address) const;
  utils::Address EntryAddress() const;

  const AdjacencyList& BlocksList() const;

 private:
  Blocks blocks_;
  AdjacencyList adjacency_;
  utils::Address entry_address_ = 0;
};

}  // namespace cfg
}  // namespace frontend
}  // namespace rosetta

#endif  // DEV_INC_FRONTEND_CONTROLFLOWGRAPH_HPP_
