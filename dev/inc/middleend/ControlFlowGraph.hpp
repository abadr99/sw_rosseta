#ifndef DEV_INC_MIDDLEEND_CONTROLFLOWGRAPH_HPP_
#define DEV_INC_MIDDLEEND_CONTROLFLOWGRAPH_HPP_

#include <map>

#include <middleend/BasicBlock.hpp>
#include <utils/Types.hpp>

namespace rosetta {
namespace middleend {
namespace cfg {

class ControlFlowGraph {
 public:
  using Blocks = std::map<frontend::utils::Address, basicblock::BasicBlock>;

  ControlFlowGraph() = default;

  void AddBlock(const basicblock::BasicBlock& block);
  void SetEntryAddress(frontend::utils::Address entry_address);

  bool HasBlock(frontend::utils::Address address) const;
  const basicblock::BasicBlock& GetBlock(frontend::utils::Address address) const;
  basicblock::BasicBlock& GetBlock(frontend::utils::Address address);
  frontend::utils::Address GetEntryAddress() const;

  const Blocks& GetBlocks() const;

 private:
  Blocks blocks_;
  frontend::utils::Address entry_address_ = 0;
};

}  // namespace cfg
}  // namespace middleend
}  // namespace rosetta

#endif  // DEV_INC_MIDDLEEND_CONTROLFLOWGRAPH_HPP_
