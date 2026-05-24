#ifndef RENDER_HAIR_BASE_COLLIDERS_HPP
#define RENDER_HAIR_BASE_COLLIDERS_HPP

#include <glm/glm.hpp>
namespace RenderHair::Physics
{
  struct Sphere
  {
    glm::vec3 center;
    float radius;
  };
}

#endif // RENDER_HAIR_BASE_COLLIDERS_HPP
