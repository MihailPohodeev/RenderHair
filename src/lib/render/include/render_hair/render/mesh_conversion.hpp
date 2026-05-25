#pragma once

#include <OgreMesh.h>

#include <vector>

#include "render_hair/core/triangle.hpp"

namespace RenderHair::Render {

  std::vector<Core::Triangle> convert_mesh_to_triangles(const Ogre::MeshPtr &mesh);

}
