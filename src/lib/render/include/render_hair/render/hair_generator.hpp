#pragma once

#include <OgreEntity.h>

#include <render_hair/render/hair_root.hpp>
#include <render_hair/render/triangle.hpp>

namespace RenderHair::Render {

  struct HairGenSettings {
    float density{500.0F};
  };

  glm::vec3 getAbsolutePosition(const HairRoot& root, const Triangle& triangle);

  glm::vec3 getInterpolatedNormal(const HairRoot& root, const Triangle& triangle);

  std::vector<HairRoot> generatePrimitiveRoots(const std::vector<Triangle>& triangles, int32_t hairs_per_triangle);

  void generateHairs(Ogre::Entity& entity, const HairGenSettings& settings);

}  // namespace RenderHair::Render
