#pragma once

#include <cstdint>
#include <glm/vec2.hpp>

namespace RenderHair {
  struct HairRoot {
    int32_t triangle_index;
    glm::vec2 baricentric_coords;
  };
}  // namespace RenderHair
