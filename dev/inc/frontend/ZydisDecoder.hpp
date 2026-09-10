#pragma once

#include <Zydis/Zydis.h>

#include <cstdint>
#include <memory>

#include "frontend/DecoderInterface.hpp"
#include "frontend/Instruction.hpp"

namespace rosetta {
namespace frontend {
namespace decode {

class ZydisInstructionDecoder : public IDecoder {
 public:
  ZydisInstructionDecoder();
  ~ZydisInstructionDecoder() = default;

  std::unique_ptr<Instruction> Decode(
      utils::Address vma, const uint8_t* buffer, utils::Size length) const override;

 private:
  static Operand ToOperand(const ZydisDecodedOperand& z_op);

  ZydisDecoder z_decoder_;
  ZydisFormatter z_formatter_;
};
}  // namespace decode
}  // namespace frontend
}  // namespace rosetta
