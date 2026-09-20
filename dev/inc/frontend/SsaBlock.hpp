#ifndef DEV_INC_FRONTEND_SSABLOCK_HPP_
#define DEV_INC_FRONTEND_SSABLOCK_HPP_

#include <vector>

#include "frontend/BasicBlock.hpp"
#include "frontend/SsaStatement.hpp"
#include "utils/Types.hpp"

namespace rosetta {
namespace frontend {
namespace ssa {

// Wraps the original machine BasicBlock (the "bb") together with the SSA
// statements lifted from it. Reading machine instructions is delegated to
// the wrapped bb; SSA statements accumulate separately here instead of
// being pushed into the bb itself.
class SsaBlock {
 public:
  using Statements = std::vector<SsaStatement>;

  SsaBlock() = default;
  explicit SsaBlock(basicblock::BasicBlock& bb);

  const basicblock::BasicBlock::Instructions& InstructionList() const;
  utils::Address StartAddress() const;
  const basicblock::BasicBlock::Addresses& Successors() const;
  const basicblock::BasicBlock::Addresses& Predecessors() const;
  basicblock::BasicBlock& Bb();

  void AddStatement(const SsaStatement& statement);
  const Statements& StatementList() const;
  Statements& StatementList();

 private:
  basicblock::BasicBlock* bb_ = nullptr;
  Statements statements_;
};

}  // namespace ssa
}  // namespace frontend
}  // namespace rosetta

#endif  // DEV_INC_FRONTEND_SSABLOCK_HPP_
