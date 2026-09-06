#ifndef DEV_INC_MIDDLEEND_CFGBUILDERINTERFACE_HPP_
#define DEV_INC_MIDDLEEND_CFGBUILDERINTERFACE_HPP_

#include <vector>

#include "frontend/Instruction.hpp"
#include "middleend/ControlFlowGraph.hpp"

namespace rosetta {
namespace middleend {
namespace cfg {

// Interface for building a ControlFlowGraph from a decoded instruction stream
class ICfgBuilder {
 public:
  explicit ICfgBuilder(const std::vector<frontend::decode::X86Instruction>& instructions);
  virtual ~ICfgBuilder();
  virtual ControlFlowGraph Build() const = 0;
 protected:
  std::vector<frontend::decode::X86Instruction> instructions_;
};

}  // namespace cfg
}  // namespace middleend
}  // namespace rosetta

#endif  // DEV_INC_MIDDLEEND_CFGBUILDERINTERFACE_HPP_
