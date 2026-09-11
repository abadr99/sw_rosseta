#ifndef DEV_INC_FRONTEND_ZYDISDECODER_HPP_
#define DEV_INC_FRONTEND_ZYDISDECODER_HPP_

#include <Zydis/Zydis.h>

#include <cstdint>
#include <memory>

#include "frontend/Instruction.hpp"
#include "frontend/DecoderInterface.hpp"

namespace rosetta {
namespace frontend {
namespace decoder {

class ZydisInstructionDecoder : public IDecoder {
 public:
  ~ZydisInstructionDecoder() = default;
  std::unique_ptr<instruction::Instruction> Decode(
    utils::Address vma, const uint8_t* buffer, utils::Size length) const override;
 private:
  instruction::InstructionOperand ToOperand(const ZydisDecodedOperand& z_op) const;
  uint32_t RegisterToNumber(ZydisRegister reg) const;
  instruction::InstructionCategory ToCategory(ZydisInstructionCategory z_category) const;
};

}  // namespace decoder
}  // namespace frontend
}  // namespace rosetta

#endif  // DEV_INC_FRONTEND_ZYDISDECODER_HPP_
