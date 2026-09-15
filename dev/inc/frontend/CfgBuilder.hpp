#ifndef DEV_INC_FRONTEND_CFGBUILDER_HPP_
#define DEV_INC_FRONTEND_CFGBUILDER_HPP_

#include <vector>

#include "frontend/ControlFlowGraph.hpp"
#include "frontend/Instruction.hpp"
#include "utils/Types.hpp"

namespace rosetta {
namespace frontend {
namespace cfg {

// Class that builds a ControlFlowGraph by splitting a decoded instruction
// stream into basic blocks and wiring successor/predecessor edges
class CfgBuilder {
 public:
  explicit CfgBuilder(
      const std::vector<frontend::instruction::Instruction>& instructions);
  ~CfgBuilder() = default;
  ControlFlowGraph Build() const;

 private:
  bool IsConditionalJump(
      const frontend::instruction::Instruction& instruction) const;
  bool IsUnconditionalJump(
      const frontend::instruction::Instruction& instruction) const;
  bool IsCall(
      const frontend::instruction::Instruction& instruction) const;
  bool IsReturn(
      const frontend::instruction::Instruction& instruction) const;
  utils::Address GetJumpTarget(
      const frontend::instruction::Instruction& instruction) const;

  std::vector<frontend::instruction::Instruction> instructions_;
};

}  // namespace cfg
}  // namespace frontend
}  // namespace rosetta

#endif  // DEV_INC_FRONTEND_CFGBUILDER_HPP_
