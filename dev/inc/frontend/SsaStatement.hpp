#ifndef DEV_INC_FRONTEND_SSASTATEMENT_HPP_
#define DEV_INC_FRONTEND_SSASTATEMENT_HPP_

#include <cstdint>
#include <vector>

#include "utils/Types.hpp"

namespace rosetta {
namespace frontend {
namespace ssa {

// TODO(@abdelrhmanatta): Add more data types as target architecture ISA support grows.
enum class SsaDataType : uint8_t {
  kNone = 0,
   kI1, kI8, kI16, kI32, kI64,
};

// Core SSA opcodes implemented as an initial baseline.
//
// TODO(@abdelrhmanatta): Add more opcodes as target architecture ISA support grows.
enum class SsaOpcode : uint8_t {
  kNone = 0,

  // Constants & Data Movement
  kConst,
  kMove,

  // Integer Arithmetic
  kSub,
  kMul,

  // Comparison
  kSle,
  kSgt,

  // Control Flow
  kCondBr,                        // conditional branch, two block targets
  kIndirectBr,                    // branch to a computed address
  kRet,

  // SSA
  kPhi,

  // Guest-state bookkeeping
  kGuestPcMarker,                 // marks original guest instruction address
  kLoadGuestStackReturnAddress,   // reads return address off guest stack
};

// Supported operand kinds within an SSA statement.
// NOTE: Adopting a strict RISC-style load-store model.
// Memory is accessed solely via `kLoad`/`kStore` using `kVirtualReg` addresses.
// Complex addressing modes (base + index * scale + disp) are lowered into explicit
// arithmetic instructions (kMul, kAdd) prior to load/store operations.
//
// TODO(@abdelrhmanatta): Add more operand variants as lowering and optimization stages require.
enum class SsaOperandType : uint8_t {
  kNone = 0,
  kConstant,     // Immediate numeric literal
  kVirtualReg,   // SSA or temporary virtual register ID
  kLabel,        // Control flow target
};

struct SsaOperand {
  SsaOperandType Type;
  SsaDataType DataType;  // 8/16/32/64 bits
  uint64_t Value;
  SsaOperand()
    : Type(SsaOperandType::kNone), DataType(SsaDataType::kNone), Value(0) {}
};

struct SsaPhiIncoming {
  utils::Address PredecessorBlock;
  SsaOperand Value;
  SsaPhiIncoming()
    : PredecessorBlock(0), Value() {}
};

class SsaStatement {
 public:
  SsaStatement() = default;

  SsaStatement(SsaOpcode opcode,
               std::vector<SsaOperand> operands,
               SsaOperand result,
               utils::Address true_target = 0,
               utils::Address false_target = 0,
               utils::Address guest_pc = 0,
               std::vector<SsaPhiIncoming> phi_incoming = {});

  SsaOpcode Opcode() const;
  const std::vector<SsaOperand>& Operands() const;
  SsaOperand Result() const;
  bool HasResult() const;

  utils::Address TrueTarget() const;
  utils::Address FalseTarget() const;
  utils::Address GuestPc() const;
  const std::vector<SsaPhiIncoming>& PhiIncomingList() const;

 private:
  SsaOpcode opcode_ = SsaOpcode::kNone;
  std::vector<SsaOperand> operands_;
  SsaOperand result_;
  utils::Address true_target_ = 0;
  utils::Address false_target_ = 0;
  utils::Address guest_pc_ = 0;
  std::vector<SsaPhiIncoming> phi_incoming_;
};

}  // namespace ssa
}  // namespace frontend
}  // namespace rosetta

#endif  // DEV_INC_FRONTEND_SSASTATEMENT_HPP_
