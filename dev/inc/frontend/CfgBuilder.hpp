#ifndef DEV_INC_FRONTEND_CFGBUILDER_HPP_
#define DEV_INC_FRONTEND_CFGBUILDER_HPP_

#include <vector>

#include "frontend/Instruction.hpp"
#include "frontend/CfgBuilderInterface.hpp"
#include "frontend/ControlFlowGraph.hpp"
#include "utils/Types.hpp"

namespace rosetta {
namespace frontend {
namespace cfg {

// Class that builds a ControlFlowGraph by splitting a decoded instruction
// stream into basic blocks and wiring successor/predecessor edges
class CfgBuilder : public ICfgBuilder {
 public:
  explicit CfgBuilder(const std::vector<frontend::decode::X86Instruction>& instructions);
  ~CfgBuilder() = default;
  ControlFlowGraph Build() const override;

 private:
  bool IsConditionalJump(ZydisMnemonic mnemonic) const;
  bool IsUnconditionalJump(ZydisMnemonic mnemonic) const;
  bool IsCall(ZydisMnemonic mnemonic) const;
  bool IsReturn(ZydisMnemonic mnemonic) const;
  frontend::utils::Address GetJumpTarget(
      const frontend::decode::X86Instruction& instruction) const;
};

}  // namespace cfg
}  // namespace frontend
}  // namespace rosetta

#endif  // DEV_INC_FRONTEND_CFGBUILDER_HPP_
