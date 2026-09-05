#ifndef DEV_INC_UTILS_TYPES_HPP_
#define DEV_INC_UTILS_TYPES_HPP_

#include <cstdint>
#include <vector>

namespace rosetta {
namespace utils {

typedef uint64_t Address;
typedef uint64_t Size;
typedef std::vector<uint8_t> Bytes;

typedef bool Flag;

}  // namespace utils
}  // namespace rosetta

#endif  // DEV_INC_UTILS_TYPES_HPP_
