#ifndef DEV_INC_FRONTEND_CPU_CPUSTATE_HPP_
#define DEV_INC_FRONTEND_CPU_CPUSTATE_HPP_

#include <cstdint>
#include <cstddef>
#include <array>
#include <string>

#include "utils/Types.hpp"

namespace rosetta {
namespace cpu {

namespace utils = ::rosetta::frontend::utils;

enum class CpuStatus {
    kRunning,
    kHalted,
    kError
};

struct Flags {
    utils::Flag cf{false};
    utils::Flag pf{false};
    utils::Flag af{false};
    utils::Flag zf{false};
    utils::Flag sf{false};
    utils::Flag of{false};

    void Reset() {
        cf = pf = af = zf = sf = of = false;
    }
};

class CpuState {
 public:
    enum RegIndex : uint8_t {
        kRax = 0, kRcx = 1, kRdx = 2, kRbx = 3,
        kRsp = 4, kRbp = 5, kRsi = 6, kRdi = 7,
        kR8  = 8, kR9  = 9, kR10 = 10, kR11 = 11,
        kR12 = 12, kR13 = 13, kR14 = 14, kR15 = 15,
        kTotalGprs = 16
    };

    CpuState();
    ~CpuState() = default;

    void Reset();

    uint64_t GetGpr(RegIndex index) const;
    void SetGpr(RegIndex index, uint64_t value);

    uint64_t GetGpr(uint8_t index) const;
    void SetGpr(uint8_t index, uint64_t value);

    utils::Address GetRip() const;
    void SetRip(utils::Address rip);

    Flags& GetFlags();
    const Flags& GetFlags() const;

    CpuStatus GetStatus() const;
    void SetStatus(CpuStatus status);

    void DumpState() const;

 private:
    std::array<uint64_t, kTotalGprs> gprs_;
    utils::Address rip_;
    Flags flags_;
    CpuStatus status_;
};

}  // namespace cpu
}  // namespace rosetta

#endif  // DEV_INC_FRONTEND_CPU_CPUSTATE_HPP_
