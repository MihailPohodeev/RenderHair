#include "render_hair/render/hair_generator.hpp"

#include <filesystem>
#include <glm/glm.hpp>
#include <random>
#include <string>

#include "render_hair/render/exceptions.hpp"
#include "render_hair/render/mesh_conversion.hpp"

namespace fs = std::filesystem;

namespace RenderHair::Render {

  namespace {
    float triangleSquare(const Triangle& triangle) {
      const glm::vec3 edge1 = triangle.vertices[1] - triangle.vertices[0];
      const glm::vec3 edge2 = triangle.vertices[2] - triangle.vertices[0];
      const glm::vec3 cross_product = glm::cross(edge1, edge2);
      return glm::length(cross_product) / 2.0F;
    }

    glm::vec3 getRandomPointInTriangle(const Triangle& triangle, float u, float v) {
      if (u + v > 1.0F) {
        u = 1.0F - u;
        v = 1.0F - v;
      }
      const float w = 1.0F - u - v;
      return triangle.vertices[0] * u + triangle.vertices[1] * v + triangle.vertices[2] * w;
    }

    glm::vec2 getInterpolatedUV(const Triangle& t, float u, float v) {
      if (u + v > 1.0F) {
        u = 1.0F - u;
        v = 1.0F - v;
      }
      const float w = 1.0F - u - v;
      return t.texture_coords[0] * u + t.texture_coords[1] * v + t.texture_coords[2] * w;
    }
  }  // namespace

  glm::vec3 getAbsolutePosition(const HairRoot& root, const Triangle& triangle) {
    const float u = root.baricentric_coords.x;
    const float v = root.baricentric_coords.y;
    const float w = 1.0F - u - v;
    return triangle.vertices[0] * u + triangle.vertices[1] * v + triangle.vertices[2] * w;
  }

  glm::vec3 getInterpolatedNormal(const HairRoot& root, const Triangle& triangle) {
    const float u = root.baricentric_coords.x;
    const float v = root.baricentric_coords.y;
    const float w = 1.0F - u - v;
    return glm::normalize(triangle.normals[0] * u + triangle.normals[1] * v + triangle.normals[2] * w);
  }

  void generateHairs(Ogre::Entity& entity, const HairGenSettings& settings) {
    const Ogre::MeshPtr& mesh = entity.getMesh();
    if (mesh == nullptr) { throw Error::RenderException{"Entity doesn't contain mesh."}; }
    const std::vector<Triangle> triangles = convert_mesh_to_triangles(mesh);
    const fs::path mesh_filename = mesh->getName();
    const std::string hair_map_filename = mesh_filename.stem().string() + "_hair_distribution_map.png";
    auto& res_group_mgr = Ogre::ResourceGroupManager::getSingleton();
    if (!res_group_mgr.resourceExistsInAnyGroup(hair_map_filename)) {
      throw Error::RenderException{"Hair distribution map not found!"};
    }

    Ogre::Image distribution_map;
    distribution_map.load(hair_map_filename, res_group_mgr.getWorldResourceGroupName());

    const size_t img_width = distribution_map.getWidth();
    const size_t img_height = distribution_map.getHeight();

    std::random_device random{};
    std::mt19937 gen(random());
    std::uniform_real_distribution<float> dis(0.0F, 1.0F);

    std::vector<glm::vec3> hair_roots;
    for (const auto& triangle : triangles) {
      const float area = triangleSquare(triangle);
      const glm::vec2 center_uv =
          (triangle.texture_coords[0] + triangle.texture_coords[1] + triangle.texture_coords[2]) / 3.0F;
      const size_t pixel_x = std::clamp(static_cast<size_t>(center_uv.x * img_width), size_t{0}, img_width - 1);
      const size_t pixel_y = std::clamp(static_cast<size_t>(center_uv.y * img_height), size_t{0}, img_height - 1);
      const Ogre::ColourValue pixel_color = distribution_map.getColourAt(pixel_x, pixel_y, 0);
      const float red_intensity = pixel_color.r;
      const float hairs_to_generate_float = area * settings.density * red_intensity;
      int hairs_to_generate = static_cast<int>(hairs_to_generate_float);
      if (dis(gen) < (hairs_to_generate_float - static_cast<float>(hairs_to_generate))) { hairs_to_generate++; }
      for (int i = 0; i < hairs_to_generate; ++i) {
        const float u = dis(gen);
        const float v = dis(gen);
        const glm::vec3 hair_root_pos = getRandomPointInTriangle(triangle, u, v);
        hair_roots.push_back(hair_root_pos);
      }
    }
  }

  std::vector<HairRoot> generatePrimitiveRoots(const std::vector<Triangle>& triangles, int32_t hairs_per_triangle) {
    std::vector<HairRoot> roots;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(0.0F, 1.0F);

    for (const auto& triangle : triangles) {
      for (int i = 0; i < hairs_per_triangle; ++i) {
        float u = dis(gen);
        float v = dis(gen);

        if (u + v > 1.0F) {
          u = 1.0F - u;
          v = 1.0F - v;
        }

        HairRoot root;
        root.triangle_index = triangle.triangle_index;
        root.baricentric_coords = glm::vec2(u, v);

        roots.push_back(root);
      }
    }

    return roots;
  }
}  // namespace RenderHair::Render
