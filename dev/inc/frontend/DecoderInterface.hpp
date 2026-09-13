#ifndef DEV_INC_FRONTEND_DECODERINTERFACE_HPP_
#define DEV_INC_FRONTEND_DECODERINTERFACE_HPP_

#include <cstdint>
#include <memory>

#include "frontend/Instruction.hpp"
#include "utils/Types.hpp"

namespace rosetta {
namespace frontend {
namespace decoder {

class IDecoder {
 public:
  virtual ~IDecoder();
  virtual std::unique_ptr<instruction::Instruction> Decode(
    utils::Address vma, const uint8_t* buffer, utils::Size length) const = 0;
};

}  // namespace decoder
}  // namespace frontend
}  // namespace rosetta

#endif  // DEV_INC_FRONTEND_DECODERINTERFACE_HPP_
