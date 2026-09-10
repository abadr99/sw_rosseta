#pragma once

#include <cstdint>
#include <memory>

#include "frontend/Instruction.hpp"
#include "utils/Types.hpp"

namespace rosetta {
namespace frontend {
namespace decode {

// Interface for decoders producing Instruction from raw bytes.
class IDecoder {
 public:
  virtual ~IDecoder();

  // Decodes one instruction at `vma`. `length` bounds reads from `buffer`.
  // Returns nullptr for null, incomplete, or invalid instruction bytes.
  virtual std::unique_ptr<Instruction> Decode(
      utils::Address vma, const uint8_t* buffer, utils::Size length) const = 0;
};

}  // namespace decode
}  // namespace frontend
}  // namespace rosetta
