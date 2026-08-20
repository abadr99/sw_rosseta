//
// Created by atta on 8/20/26.
//

#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "types.hpp"

namespace loader {
class ElfLoader {
 public:
  [[nodiscard]] static std::pair<std::optional<ElfLoader>, Status>
  load(const std::string& path);

  [[nodiscard]] std::pair<std::optional<uint64_t>, Status>
  vma_to_offset(uint64_t vma) const noexcept;

  [[nodiscard]] uint64_t get_entry_point() const noexcept
  { return entry_point_; }
  [[nodiscard]] const std::vector<uint8_t>& get_text_section() const noexcept
  { return text_section_; }

 private:
  ElfLoader(uint64_t entry_point, uint64_t text_vma,
            std::vector<uint8_t> text_section) noexcept;

  uint64_t entry_point_;
  uint64_t text_vma_;
  std::vector<uint8_t> text_section_;
};

}  // namespace loader
