#pragma once

#include <Zydis/Zydis.h>
#include <cstddef>
#include <cstdint>
#include <optional>

#include "Instructions.hpp"

class Decoder {
 private:
  ZydisDecoder z_decoder;
  ZydisFormatter z_formatter;

 public:
  Decoder();

  // Decodes one instruction at `vma`. `length` bounds reads from `buffer`.
  // Returns std::nullopt for null, incomplete, or invalid instruction bytes.
  std::optional<X86Instruction> decode(uint64_t vma, const uint8_t* buffer,
                                       size_t length);
};
