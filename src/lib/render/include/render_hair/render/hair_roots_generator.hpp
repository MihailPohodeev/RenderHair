#pragma once

#include <OgreEntity.h>

#include <render_hair/render/hair_root.hpp>
#include <render_hair/render/triangle.hpp>

namespace RenderHair {

  struct HairRootsGenerator {
    static std::vector<Ogre::Vector4> generateRandomPoints(Ogre::MeshPtr mesh, int32_t hairs_per_polygon,
                                                           int32_t nodes_per_hair, float total_hair_length);

    static std::vector<Ogre::Vector4> generateWithMap(const Ogre::MeshPtr& mesh, int32_t nodes_per_hair,
                                                      float hair_length, int32_t max_hairs_per_unit_area);
  };

  // glm::vec3 getAbsolutePosition(const HairRoot& root, const Triangle& triangle);
  // glm::vec3 getInterpolatedNormal(const HairRoot& root, const Triangle& triangle);
  // std::vector<HairRoot> generatePrimitiveRoots(Ogre::MeshPtr mesh, int32_t hairs_per_triangle);
  // void generateHairs(Ogre::Entity& entity, const HairGenSettings& settings);

}  // namespace RenderHair
