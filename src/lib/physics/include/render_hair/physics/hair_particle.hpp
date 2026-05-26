#pragma once

#include <glm/vec4.hpp>

namespace RenderHair::Physics {
  struct HairParticle {
    glm::vec4 current_pos;
    glm::vec4 previous_pos;
  };

  constexpr size_t EXPECTED_STRUCT_SIZE{32};
  static_assert(sizeof(HairParticle) == EXPECTED_STRUCT_SIZE, "Size of `HairParticle` struct should be 32 bytes.");
}  // namespace RenderHair::Physics
