#ifndef DEV_INC_FRONTEND_CONTROLFLOWGRAPH_HPP_
#define DEV_INC_FRONTEND_CONTROLFLOWGRAPH_HPP_

#include <map>

#include "frontend/BasicBlock.hpp"
#include "utils/Types.hpp"

namespace rosetta {
namespace frontend {
namespace cfg {

class ControlFlowGraph {
 public:
  using Blocks = std::map<frontend::utils::Address, frontend::basicblock::BasicBlock>;

  ControlFlowGraph() = default;

  void AddBlock(const frontend::basicblock::BasicBlock& block);
  void SetEntryAddress(frontend::utils::Address entry_address);

  bool HasBlock(frontend::utils::Address address) const;
  const frontend::basicblock::BasicBlock& Block(frontend::utils::Address address) const;
  frontend::utils::Address EntryAddress() const;

  const Blocks& BlocksList() const;

 private:
  Blocks blocks_;
  frontend::utils::Address entry_address_ = 0;
};

}  // namespace cfg
}  // namespace frontend
}  // namespace rosetta

#endif  // DEV_INC_FRONTEND_CONTROLFLOWGRAPH_HPP_
