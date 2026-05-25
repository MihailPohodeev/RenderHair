#ifndef RENDER_HAIR_HAIR_ROOTS_ALLOCATION_HPP
#define RENDER_HAIR_HAIR_ROOTS_ALLOCATION_HPP

#include <array>
#include <functional>
#include <glm/glm.hpp>
#include <span>
#include <vector>

namespace RenderHair {

struct Triangle {
  std::array<glm::vec3, 3> vertexes;
  std::array<glm::vec3, 3> normals;
  std::array<glm::vec2, 3> uv;
};

struct HairRoot {
  glm::vec3 position;
  glm::vec3 direction;
  float length;
};

using SamplerFunc = std::function<glm::vec4(const glm::vec2&)>;

struct HairRootsAllocation {
  static std::vector<HairRoot> allocate(std::span<const Triangle> triangles,
                                        const SamplerFunc& sampler,
                                        float max_density, float max_length);

 private:
  static std::vector<HairRoot> allocateRootsOnTriangle(
      const Triangle& triangle, const SamplerFunc& sampler, float max_density,
      float max_length);
};

}  // namespace RenderHair

#endif  // RENDER_HAIR_HAIR_ROOT_ALLOCATION_HPP
