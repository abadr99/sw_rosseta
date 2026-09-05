#ifndef DEV_INC_FRONTEND_DECODERINTERFACE_HPP_
#define DEV_INC_FRONTEND_DECODERINTERFACE_HPP_

#include "utils/Types.hpp"

namespace rosetta {
namespace frontend {
namespace decoder {

class DecoderInterface {
 public:
  virtual ~DecoderInterface() = default;
};

}  // namespace decoder
}  // namespace frontend
}  // namespace rosetta

#endif  // DEV_INC_FRONTEND_DECODERINTERFACE_HPP_