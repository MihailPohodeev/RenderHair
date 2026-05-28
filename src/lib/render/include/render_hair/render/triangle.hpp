#pragma once

#include <array>
#include <cstdint>
#include <glm/glm.hpp>

namespace RenderHair {
  struct Triangle {
    int32_t submesh_index{0};
    int32_t triangle_index{0};
    std::array<glm::vec3, 3> vertices{};
    std::array<glm::vec3, 3> normals{};
    std::array<glm::vec2, 3> texture_coords{};

    [[nodiscard]] float getSquare() const {
      const glm::vec3 edge_1 = vertices[1] - vertices[0];
      const glm::vec3 edge_2 = vertices[2] - vertices[0];
      return 0.5F * glm::length(glm::cross(edge_1, edge_2));
    }
  };
}  // namespace RenderHair
