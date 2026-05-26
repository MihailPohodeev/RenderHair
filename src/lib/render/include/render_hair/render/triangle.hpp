#pragma once

#include <array>
#include <cstdint>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace RenderHair::Render {
  struct Triangle {
    int32_t submesh_index{0};
    int32_t triangle_index{0};
    std::array<glm::vec3, 3> vertices{};
    std::array<glm::vec3, 3> normals{};
    std::array<glm::vec2, 3> texture_coords{};
  };
}  // namespace RenderHair::Render
