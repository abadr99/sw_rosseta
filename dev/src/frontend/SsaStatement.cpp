#include "frontend/SsaStatement.hpp"

#include <utility>

namespace rosetta {
namespace frontend {
namespace ssa {

SsaStatement::SsaStatement(SsaOpcode opcode,
                            std::vector<SsaOperand> operands,
                            SsaOperand result,
                            utils::Address true_target,
                            utils::Address false_target,
                            utils::Address guest_pc,
                            std::vector<SsaPhiIncoming> phi_incoming)
    : opcode_(opcode),
      operands_(std::move(operands)),
      result_(result),
      true_target_(true_target),
      false_target_(false_target),
      guest_pc_(guest_pc),
      phi_incoming_(std::move(phi_incoming)) {}

SsaOpcode SsaStatement::Opcode() const {
  return opcode_;
}

const std::vector<SsaOperand>& SsaStatement::Operands() const {
  return operands_;
}

SsaOperand SsaStatement::Result() const {
  return result_;
}

bool SsaStatement::HasResult() const {
  return result_.Type != SsaOperandType::kNone;
}

utils::Address SsaStatement::TrueTarget() const {
  return true_target_;
}

utils::Address SsaStatement::FalseTarget() const {
  return false_target_;
}

utils::Address SsaStatement::GuestPc() const {
  return guest_pc_;
}

const std::vector<SsaPhiIncoming>& SsaStatement::PhiIncomingList() const {
  return phi_incoming_;
}

}  // namespace ssa
}  // namespace frontend
}  // namespace rosetta
