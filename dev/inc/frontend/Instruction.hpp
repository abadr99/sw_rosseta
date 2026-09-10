#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "utils/Types.hpp"

namespace rosetta {
namespace frontend {
namespace decode {

enum class OperandKind : uint8_t {
  kUnused = 0,
  kRegister,
  kMemory,
  kImmediate,
};

// One instruction operand. Which fields are meaningful depends on OperandKind:
//   kRegister:  reg
//   kImmediate: immediate
//   kMemory:    reg (base), index, scale, displacement
struct Operand {
  OperandKind kind = OperandKind::kUnused;
  std::string reg;
  std::string index;
  uint8_t scale = 0;
  int64_t displacement = 0;
  int64_t immediate = 0;
  uint64_t size_bits = 0;
};

class Instruction {
 public:
  explicit Instruction(utils::Address addr, std::string mnemonic, uint8_t length,
                       std::string text, uint32_t flags_read, uint32_t flags_written,
                       std::vector<Operand> operands);

  utils::Address GetAddress() const { return address_; }
  const std::string& GetMnemonic() const { return mnemonic_; }
  uint8_t GetLength() const { return length_; }
  const std::string& GetText() const { return text_; }

  uint32_t GetFlagsRead() const { return flags_read_; }
  uint32_t GetFlagsWritten() const { return flags_written_; }

  uint8_t GetOperandCount() const { return static_cast<uint8_t>(operands_.size()); }
  const Operand& GetOperand(uint8_t index) const;

 private:
  utils::Address address_;
  std::string mnemonic_;
  uint8_t length_;
  std::string text_;

  uint32_t flags_read_;
  uint32_t flags_written_;

  std::vector<Operand> operands_;
};

}  // namespace decode
}  // namespace frontend
}  // namespace rosetta
