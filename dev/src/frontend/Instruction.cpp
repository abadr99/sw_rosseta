#include <vector>
#include <string>
#include <utility>

#include "frontend/Instruction.hpp"

using namespace rosetta::frontend;  // NOLINT
using namespace rosetta::frontend::decode;  // NOLINT

Instruction::Instruction(utils::Address addr, std::string mnemonic, uint8_t length,
                         std::string text, uint32_t flags_read, uint32_t flags_written,
                         std::vector<Operand> operands)
  : address_(addr)
  , mnemonic_(std::move(mnemonic))
  , length_(length)
  , text_(std::move(text))
  , flags_read_(flags_read)
  , flags_written_(flags_written)
  , operands_(std::move(operands)) {
}

const Operand& Instruction::GetOperand(uint8_t index) const {
  return operands_.at(index);
}
