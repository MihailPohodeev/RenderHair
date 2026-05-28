#pragma once

#include <OgreMesh.h>

#include <vector>

#include "render_hair/render/triangle.hpp"

namespace RenderHair {

  [[nodiscard]]
  std::vector<Triangle> convert_mesh_to_triangles(Ogre::MeshPtr mesh);

}
