#pragma once

#include <OgreMesh.h>

#include <vector>

#include "render_hair/render/triangle.hpp"

namespace RenderHair::Render {

  [[nodiscard]]
  std::vector<Triangle> convert_mesh_to_triangles(const Ogre::MeshPtr &mesh);

}
