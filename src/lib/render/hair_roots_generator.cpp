#include "render_hair/render/hair_roots_generator.hpp"

#include <OgreEntity.h>
#include <OgreImage.h>
#include <OgreLogManager.h>
#include <OgreMesh.h>
#include <OgreResourceGroupManager.h>
#include <OgreSubMesh.h>

#include <cmath>
#include <filesystem>
#include <random>
#include <vector>

#include "render_hair/render/exceptions.hpp"
#include "render_hair/render/mesh_conversion.hpp"

namespace fs = std::filesystem;

namespace RenderHair {
  std::vector<Ogre::Vector4> HairRootsGenerator::generateRandomPoints(Ogre::MeshPtr mesh, int32_t hairs_per_polygon,
                                                                      int32_t nodes_per_hair, float total_hair_length) {
    std::vector<Ogre::Vector4> all_hair_nodes;
    if (!mesh || hairs_per_polygon <= 0 || nodes_per_hair < 2) { return all_hair_nodes; }

    auto triangles = RenderHair::convert_mesh_to_triangles(mesh);
    if (triangles.empty()) { return all_hair_nodes; }

    const size_t total_hairs = triangles.size() * static_cast<size_t>(hairs_per_polygon);
    const size_t total_nodes_count = total_hairs * static_cast<size_t>(nodes_per_hair);
    all_hair_nodes.reserve(total_nodes_count);

    const float segment_length = total_hair_length / static_cast<float>(nodes_per_hair - 1);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);

    for (const auto& tri : triangles) {
      Ogre::Vector3 A(tri.vertices[0].x, tri.vertices[0].y, tri.vertices[0].z);
      Ogre::Vector3 B(tri.vertices[1].x, tri.vertices[1].y, tri.vertices[1].z);
      Ogre::Vector3 C(tri.vertices[2].x, tri.vertices[2].y, tri.vertices[2].z);

      Ogre::Vector3 nA(tri.normals[0].x, tri.normals[0].y, tri.normals[0].z);
      Ogre::Vector3 nB(tri.normals[1].x, tri.normals[1].y, tri.normals[1].z);
      Ogre::Vector3 nC(tri.normals[2].x, tri.normals[2].y, tri.normals[2].z);

      for (int32_t h = 0; h < hairs_per_polygon; ++h) {
        float r1 = dist(gen);
        float r2 = dist(gen);
        float sqrt_r1 = std::sqrt(r1);
        float u = 1.0f - sqrt_r1;
        float v = r2 * sqrt_r1;
        float w = 1.0f - u - v;

        Ogre::Vector3 root_pos = (u * A) + (v * B) + (w * C);
        Ogre::Vector3 blended_normal = (u * nA) + (v * nB) + (w * nC);
        blended_normal.normalise();

        root_pos += blended_normal * 0.001F;

        for (int32_t k = 0; k < nodes_per_hair; ++k) {
          Ogre::Vector3 node_pos = root_pos + (blended_normal * (static_cast<float>(k) * segment_length));
          all_hair_nodes.emplace_back(node_pos.x, node_pos.y, node_pos.z, static_cast<float>(k));
        }
      }
    }

    return all_hair_nodes;
  }

  std::vector<Ogre::Vector4> HairRootsGenerator::generateWithMap(const Ogre::MeshPtr& mesh, int32_t nodes_per_hair,
                                                                 float hair_length, int32_t max_hairs_per_unit_area) {
    std::vector<Ogre::Vector4> all_hair_nodes;

    if (nodes_per_hair < 2) {
      std::string error_message = "RenderHair Error: Количество узлов должно быть минимум 2 (корень + кончик).";
      Ogre::LogManager::getSingleton().logMessage(error_message, Ogre::LogMessageLevel::LML_CRITICAL);
      throw Error::RenderException{error_message};
    }

    std::string mesh_name = mesh->getName();
    std::string stem_mesh_name = fs::path{mesh_name}.stem();
    std::string textureName = stem_mesh_name + "_hair_allocation_map.png";

    Ogre::Image allocation_map;
    try {
      allocation_map.load(textureName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    } catch (Ogre::Exception& e) {
      std::string error_message = "RenderHair Error: Не удалось загрузить карту " + textureName;
      Ogre::LogManager::getSingleton().logMessage(error_message, Ogre::LogMessageLevel::LML_CRITICAL);
      throw Error::RenderException{error_message};
    }

    Ogre::LogManager::getSingleton().logMessage("successful.");

    size_t tex_width = allocation_map.getWidth();
    size_t tex_height = allocation_map.getHeight();

    std::random_device random{};
    std::mt19937 gen(random());
    std::uniform_real_distribution<float> dis(0.0f, 1.0f);

    auto triangles = convert_mesh_to_triangles(mesh);

    for (const auto& triangle : triangles) {
      const float triangle_square = triangle.getSquare();
      float float_attempts = triangle_square * static_cast<float>(max_hairs_per_unit_area);
      int32_t attempts = static_cast<int32_t>(std::floor(float_attempts));
      if (dis(gen) < (float_attempts - static_cast<float>(attempts))) { ++attempts; }

      for (int32_t a = 0; a < attempts; ++a) {
        float r1 = dis(gen);
        float r2 = dis(gen);
        if (r1 + r2 > 1.0f) {
          r1 = 1.0f - r1;
          r2 = 1.0f - r2;
        }
        float r3 = 1.0f - r1 - r2;

        glm::vec2 interpolated_uv =
            triangle.texture_coords[0] * r1 + triangle.texture_coords[1] * r2 + triangle.texture_coords[2] * r3;
        interpolated_uv = glm::clamp(interpolated_uv, glm::vec2(0.0f), glm::vec2(1.0f));

        size_t pixel_x = static_cast<size_t>(interpolated_uv.x * static_cast<float>(tex_width - 1));
        size_t pixel_y = static_cast<size_t>((1.0f - interpolated_uv.y) * static_cast<float>(tex_height - 1));
        pixel_x = std::clamp(pixel_x, size_t(0), tex_width - 1);
        pixel_y = tex_height - std::clamp(pixel_y, size_t(0), tex_height - 1);
        float density = allocation_map.getColourAt(pixel_x, pixel_y, 0).r;
        if (dis(gen) > density) { continue; }

        glm::vec3 root_pos = triangle.vertices[0] * r1 + triangle.vertices[1] * r2 + triangle.vertices[2] * r3;
        glm::vec3 normal = triangle.normals[0] * r1 + triangle.normals[1] * r2 + triangle.normals[2] * r3;
        normal = glm::normalize(normal);

        float current_hair_length = hair_length;
        float segment_length = current_hair_length / static_cast<float>(nodes_per_hair - 1);

        for (int32_t node_idx = 0; node_idx < nodes_per_hair; ++node_idx) {
          glm::vec3 node_pos = root_pos + normal * (segment_length * static_cast<float>(node_idx));
          float weight = static_cast<float>(node_idx) / nodes_per_hair;
          all_hair_nodes.emplace_back(node_pos.x, node_pos.y, node_pos.z, weight);
        }
      }
    }

    return all_hair_nodes;
  }
}  // namespace RenderHair
