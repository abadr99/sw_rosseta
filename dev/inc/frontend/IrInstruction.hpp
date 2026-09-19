#ifndef DEV_INC_FRONTEND_IRINSTRUCTION_HPP_
#define DEV_INC_FRONTEND_IRINSTRUCTION_HPP_

#include <cstdint>
#include <vector>

#include "utils/Types.hpp"

namespace rosetta {
namespace frontend {
namespace ir {

// TODO(@abdelrhmanatta): Add more data types as target architecture ISA support grows.
enum class IrDataType : uint8_t {
  kNone = 0,
  kI8, kI16, kI32, kI64,
};

// Core intermediate representation opcodes implemented as an initial baseline.
//
// TODO(@abdelrhmanatta): Add more opcodes as target architecture ISA support grows.
enum class IrOpcode : uint8_t {
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

// Supported operand kinds within an intermediate representation instruction.
// NOTE: Adopting a strict RISC-style load-store model.
// Memory is accessed solely via `kLoad`/`kStore` using `kVirtualReg` addresses.
// Complex addressing modes (base + index * scale + disp) are lowered into explicit
// arithmetic instructions (kMul, kAdd) prior to load/store operations.
//
// TODO(@abdelrhmanatta): Add more operand variants as lowering and optimization stages require.
enum class IrOperandType : uint8_t {
  kNone = 0,
  kConstant,     // Immediate numeric literal
  kVirtualReg,   // SSA or temporary virtual register ID
  kLabel,        // Control flow target
};

struct IrOperand {
  IrOperandType Type;
  IrDataType DataType;  // 8/16/32/64 bits
  uint64_t Value;
  IrOperand()
    : Type(IrOperandType::kNone), DataType(IrDataType::kNone), Value(0) {}
};

struct PhiIncoming {
  utils::Address PredecessorBlock;
  IrOperand Value;
  PhiIncoming()
    : PredecessorBlock(0), Value() {}
};

class IrInstruction {
 public:
  IrInstruction() = default;

  IrInstruction(IrOpcode opcode,
                std::vector<IrOperand> operands,
                IrOperand result,
                utils::Address true_target = 0,
                utils::Address false_target = 0,
                utils::Address guest_pc = 0,
                std::vector<PhiIncoming> phi_incoming = {});

  IrOpcode Opcode() const;
  const std::vector<IrOperand>& Operands() const;
  IrOperand Result() const;
  bool HasResult() const;

  utils::Address TrueTarget() const;
  utils::Address FalseTarget() const;
  utils::Address GuestPc() const;
  const std::vector<PhiIncoming>& PhiIncomingList() const;

 private:
  IrOpcode opcode_ = IrOpcode::kNone;
  std::vector<IrOperand> operands_;
  IrOperand result_;
  utils::Address true_target_ = 0;
  utils::Address false_target_ = 0;
  utils::Address guest_pc_ = 0;
  std::vector<PhiIncoming> phi_incoming_;
};

}  // namespace ir
}  // namespace frontend
}  // namespace rosetta

#endif  // DEV_INC_FRONTEND_IRINSTRUCTION_HPP_
