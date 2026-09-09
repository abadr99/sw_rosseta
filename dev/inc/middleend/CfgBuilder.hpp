#ifndef DEV_INC_MIDDLEEND_CFGBUILDER_HPP_
#define DEV_INC_MIDDLEEND_CFGBUILDER_HPP_

#include <vector>

#include "frontend/Instruction.hpp"
#include "middleend/CfgBuilderInterface.hpp"
#include "middleend/ControlFlowGraph.hpp"
#include "utils/Types.hpp"

namespace rosetta {
namespace middleend {
namespace cfg {

class CfgBuilder : public ICfgBuilder {
 public:
  explicit CfgBuilder(const std::vector<frontend::decode::X86Instruction>& instructions);
  ~CfgBuilder() override = default;

  ControlFlowGraph Build() const override;

 private:
  bool IsConditionalJump(ZydisMnemonic mnemonic) const;
  bool IsUnconditionalJump(ZydisMnemonic mnemonic) const;
  bool IsReturn(ZydisMnemonic mnemonic) const;
  bool IsDirectBranch(const frontend::decode::X86Instruction& instruction) const;
  frontend::utils::Address GetJumpTarget(
      const frontend::decode::X86Instruction& instruction) const;
};

}  // namespace cfg
}  // namespace middleend
}  // namespace rosetta

#endif  // DEV_INC_MIDDLEEND_CFGBUILDER_HPP_
