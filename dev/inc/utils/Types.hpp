#ifndef DEV_INC_UTILS_TYPES_H_
#define DEV_INC_UTILS_TYPES_H_

#include <cstdint>
#include <vector>

namespace rosetta {
namespace frontend {
namespace utils {

typedef uint64_t Address;
typedef uint64_t Size;
typedef std::vector<uint8_t> Bytes;

typedef bool Flag;

}  // namespace utils
}  // namespace frontend
}  // namespace rosetta

#endif  // DEV_INC_UTILS_TYPES_H_
