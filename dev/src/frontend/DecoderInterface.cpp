#include "frontend/DecoderInterface.hpp"

#include <vector>
#include <utility>

using namespace rosetta::frontend;  // NOLINT
using namespace rosetta::frontend::decoder;  // NOLINT

IDecoder::~IDecoder() = default;

std::vector<instruction::Instruction> IDecoder::DecodeAll(
    utils::Address vma, const uint8_t* buffer, utils::Size length) const {
  std::vector<instruction::Instruction> instructions;

  utils::Size consumed = 0;
  while (consumed < length) {
    auto inst = Decode(vma + consumed, buffer + consumed, length - consumed);
    if (!inst) {
      break;
    }
    const uint32_t inst_size = inst->Size();
    if (inst_size == 0) {
      break;
    }
    consumed += inst_size;
    instructions.push_back(std::move(*inst));
  }

  return instructions;
}
