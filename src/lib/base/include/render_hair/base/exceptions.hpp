#pragma once

#include <algorithm>
#include <array>
#include <exception>
#include <string_view>

namespace RenderHair::Base {
  class Exception : public std::exception {
    constexpr static size_t max_message_length = 256;
    std::array<char, max_message_length> buffer_;

   public:
    explicit Exception(std::string_view message) {
      const size_t bytes_for_write_count = std::min(message.size(), max_message_length - 1);
      std::copy_n(message.begin(), bytes_for_write_count, buffer_.begin());
      buffer_[bytes_for_write_count] = '\0';
    }

    [[nodiscard]] const char* what() const noexcept override { return buffer_.data(); }
  };
}  // namespace RenderHair::Base
