#include "hair_roots_allocation.hpp"

#include <random>

namespace {
std::mt19937& get_rng() {
  static std::random_device rand;
  static std::mt19937 gen(rand());
  return gen;
}

float random_float() {
  std::uniform_real_distribution<float> dis(0.0F, 1.0F);
  return dis(get_rng());
}
}  // namespace

namespace RenderHair {

std::vector<HairRoot> HairRootsAllocation::allocate(
    std::span<const Triangle> triangles, const SamplerFunc& sampler,
    float max_density, float max_length) {
  std::vector<HairRoot> result;

  for (const auto& triangle : triangles) {
    auto allocated_roots =
        allocateRootsOnTriangle(triangle, sampler, max_density, max_length);
    result.insert(result.end(), allocated_roots.begin(), allocated_roots.end());
  }

  return result;
}

std::vector<HairRoot> HairRootsAllocation::allocateRootsOnTriangle(
    const Triangle& triangle, const SamplerFunc& sampler, float max_density,
    float max_length) {
  std::vector<HairRoot> roots;

  const glm::vec3 edge1 = triangle.vertexes[1] - triangle.vertexes[0];
  const glm::vec3 edge2 = triangle.vertexes[2] - triangle.vertexes[0];
  const float area = 0.5F * glm::length(glm::cross(edge1, edge2));

  const int num_samples = static_cast<int>(area * max_density);

  for (int i = 0; i < num_samples; ++i) {
    const float rand_1 = random_float();
    const float rand_2 = random_float();

    const float sqrt_r1 = std::sqrt(rand_1);

    // NOLINTBEGIN(readability-identifier-length)
    const float w = 1.0F - sqrt_r1;
    const float u = sqrt_r1 * (1.0F - rand_2);
    const float v = sqrt_r1 * rand_2;
    // NOLINTEND(readability-identifier-length)

    const glm::vec2 interpolated_uv =
        w * triangle.uv[0] + u * triangle.uv[1] + v * triangle.uv[2];

    const glm::vec4 map_data = sampler(interpolated_uv);
    const float density_factor = map_data.r;
    const float length_factor = map_data.b;

    if (random_float() < density_factor) {
      const glm::vec3 pos = w * triangle.vertexes[0] +
                            u * triangle.vertexes[1] + v * triangle.vertexes[2];
      glm::vec3 dir = w * triangle.normals[0] + u * triangle.normals[1] +
                      v * triangle.normals[2];
      dir = glm::normalize(dir);
      roots.push_back({.position = pos,
                       .direction = dir,
                       .length = length_factor * max_length});
    }
  }

  return roots;
}

}  // namespace RenderHair
