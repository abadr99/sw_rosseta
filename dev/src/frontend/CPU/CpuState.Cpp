#include "frontend/CPU/CpuState.hpp"

#include <iostream>
#include <iomanip>
#include <stdexcept>

namespace rosetta {
namespace cpu {

namespace utils = ::rosetta::frontend::utils;

CpuState::CpuState() {
    Reset();
}

void CpuState::Reset() {
    gprs_.fill(0);
    rip_ = 0;
    flags_.Reset();
    status_ = CpuStatus::kRunning;
}

uint64_t CpuState::GetGpr(RegIndex index) const {
    return gprs_.at(static_cast<size_t>(index));
}

void CpuState::SetGpr(RegIndex index, uint64_t value) {
    gprs_.at(static_cast<size_t>(index)) = value;
}

uint64_t CpuState::GetGpr(uint8_t index) const {
    if (index >= kTotalGprs) {
        throw std::out_of_range("GPR index out of range: " + std::to_string(index));
    }
    return gprs_[index];
}

void CpuState::SetGpr(uint8_t index, uint64_t value) {
    if (index >= kTotalGprs) {
        throw std::out_of_range("GPR index out of range: " + std::to_string(index));
    }
    gprs_[index] = value;
}

utils::Address CpuState::GetRip() const {
    return rip_;
}

void CpuState::SetRip(utils::Address rip) {
    rip_ = rip;
}

Flags& CpuState::GetFlags() {
    return flags_;
}

const Flags& CpuState::GetFlags() const {
    return flags_;
}

CpuStatus CpuState::GetStatus() const {
    return status_;
}

void CpuState::SetStatus(CpuStatus status) {
    status_ = status;
}

void CpuState::DumpState() const {
    std::cout << "=== CPU State ===" << std::endl;
    std::cout << std::hex << std::setfill('0');
    std::cout << "RIP: 0x" << std::setw(16) << rip_ << std::endl;

    const char* reg_names[kTotalGprs] = {
        "RAX", "RCX", "RDX", "RBX", "RSP", "RBP", "RSI", "RDI",
        "R8 ", "R9 ", "R10", "R11", "R12", "R13", "R14", "R15"
    };

    for (size_t i = 0; i < kTotalGprs; i += 2) {
        std::cout << reg_names[i] << ": 0x" << std::setw(16) << gprs_[i] << "  "
                  << reg_names[i + 1] << ": 0x" << std::setw(16) << gprs_[i + 1]
                  << std::endl;
    }

    std::cout << std::dec;
    std::cout << "Flags: [CF: " << flags_.cf << " | PF: " << flags_.pf
              << " | AF: " << flags_.af << " | ZF: " << flags_.zf
              << " | SF: " << flags_.sf << " | OF: " << flags_.of << "]"
              << std::endl;
}

}  // namespace cpu
}  // namespace rosetta