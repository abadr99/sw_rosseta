#ifndef DEV_INC_FRONTEND_CFGBUILDERINTERFACE_HPP_
#define DEV_INC_FRONTEND_CFGBUILDERINTERFACE_HPP_

#include <vector>

#include "frontend/Instruction.hpp"
#include "frontend/ControlFlowGraph.hpp"

namespace rosetta {
namespace frontend {
namespace cfg {

// Interface for building a ControlFlowGraph from a decoded instruction stream
class ICfgBuilder {
 public:
  explicit ICfgBuilder(const std::vector<frontend::instruction::Instruction>& instructions);
  virtual ~ICfgBuilder();
  virtual ControlFlowGraph Build() const = 0;
 protected:
  std::vector<frontend::instruction::Instruction> instructions_;
};

}  // namespace cfg
}  // namespace frontend
}  // namespace rosetta

#endif  // DEV_INC_FRONTEND_CFGBUILDERINTERFACE_HPP_
