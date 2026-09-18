#ifndef DEV_INC_FRONTEND_IRFUNCTION_HPP_
#define DEV_INC_FRONTEND_IRFUNCTION_HPP_

#include <map>
#include <list>

#include "utils/Types.hpp"
#include "frontend/IrBasicBlock.hpp"

namespace rosetta {
namespace frontend {
namespace ir {

class IrFunction {
 public:
  using Blocks = std::map<utils::Address, IrBasicBlock>;
  using AdjacencyList = std::map<utils::Address, std::list<utils::Address>>;

  explicit IrFunction();
  ~IrFunction() = default;

  void AddBlock(const IrBasicBlock& block);
  void AddEdge(utils::Address from, utils::Address to);
  void SetEntryAddress(utils::Address entry_address);

  bool HasBlock(utils::Address address) const;
  const AdjacencyList& BlocksList() const;
  const IrBasicBlock& Block(utils::Address address) const;
  utils::Address EntryAddress() const;
  
 private:
  Blocks blocks_;
  AdjacencyList adjacency_;
  utils::Address entry_address_ = 0;
};
}  // namespace rosetta
}  // namespace frontend
}  // namespace ir

#endif  // DEV_INC_FRONTEND_IRFUNCTION_HPP_
