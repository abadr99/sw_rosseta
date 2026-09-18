#ifndef DEV_INC_FRONTEND_IRINSTRUCTION_HPP_
#define DEV_INC_FRONTEND_IRINSTRUCTION_HPP_

#include <cstdint>
#include <vector>

namespace rosetta {
namespace frontend {
namespace ir {

// Core intermediate representation opcodes implemented as an initial baseline.
//
// TODO(@abdelrhmanatta): Add more opcodes as target architecture ISA support grows.
enum class IrOpcode : uint8_t {
  kNone = 0,

  // Integer Arithmetic
  kAdd,
  kSub,
  kMul,
  kDiv,

  // Bitwise & Shifts
  kAnd,
  kOr,
  kXor,
  kShl,
  kShr,

  // Memory Operations
  kLoad,
  kStore,

  // Data Movement & SSA
  kMove,
  kPhi,

  // Comparison & Control Flow
  kCmp,
  kBranch,  // Conditional branch
  kJump,    // Unconditional jump
  kCall,
  kRet,
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
  IrOperandType Type = IrOperandType::kNone;
  uint8_t SizeBits;  // 8/16/32/64 bits
  uint64_t Value;
  IrOperand()
      : Type(IrOperandType::kNone), SizeBits(0), Value(0) {}
};

class IrInstruction {
 public:
  IrInstruction() = default;
  IrInstruction(IrOpcode opcode, std::vector<IrOperand> operands, IrOperand result);

  IrOpcode Opcode() const;
  const std::vector<IrOperand>& Operands() const;
  IrOperand Result() const;

  bool HasResult() const;

 private:
  // Three-Address Code (TAC) Format: result = opcode operands
  IrOpcode opcode_ = IrOpcode::kNone;
  std::vector<IrOperand> operands_;
  IrOperand result_;
};

}  // namespace ir
}  // namespace frontend
}  // namespace rosetta

#endif  // DEV_INC_FRONTEND_IRINSTRUCTION_HPP_
