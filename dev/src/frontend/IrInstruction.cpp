#include "frontend/IrInstruction.hpp"

#include <utility>

namespace rosetta {
namespace frontend {
namespace ir {

IrInstruction::IrInstruction(IrOpcode opcode, std::vector<IrOperand> operands,
                             IrOperand result)
    : opcode_(opcode), operands_(std::move(operands)), result_(result) {}

IrOpcode IrInstruction::Opcode() const {
  return opcode_;
}

const std::vector<IrOperand>& IrInstruction::Operands() const {
  return operands_;
}

IrOperand IrInstruction::Result() const {
  return result_;
}

bool IrInstruction::HasResult() const {
  return result_.type != IrOperandType::kNone;
}

}  // namespace ir
}  // namespace frontend
}  // namespace rosetta
