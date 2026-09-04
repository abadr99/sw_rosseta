#ifndef UTILS_TYPES_H
#define UTILS_TYPES_H

#include <cstdint>
#include <vector>

namespace rosetta {
namespace frontend {
namespace utils {

typedef uint64_t Address;
typedef uint64_t Size;
typedef std::vector<uint8_t> Bytes;

typedef bool Flag;

}}} // namespace rosetta::frontend::utils
#endif