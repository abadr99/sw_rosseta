#ifndef UTILS_MACROS_H
#define UTILS_MACROS_H

#include <cstdio>
#include <cstdlib>

namespace rosetta {
namespace frontend {
namespace utils {

[[noreturn]] inline void Unreachable(const char* message, const char* file,
                                     int line) {
  std::fprintf(stderr, "UNREACHABLE: %s (%s:%d)\n",
               message != nullptr ? message : "", file, line);
  std::abort();
}

}  // namespace utils
}  // namespace frontend
}  // namespace rosetta

#define UNREACHABLE(message) \
  ::rosetta::frontend::utils::Unreachable((message), __FILE__, __LINE__)

#endif
