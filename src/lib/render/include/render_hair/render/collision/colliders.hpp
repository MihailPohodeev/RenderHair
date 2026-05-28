#pragma once

#include <OgreMatrix4.h>
#include <OgreVector3.h>

#include <variant>

namespace RenderHair::Collider {

  struct Sphere {
    Ogre::Vector3 center;
    float radius{};
  };

  struct SphereCollider {
    Sphere sphere;
    Sphere* world_sphere;
    void update(const Ogre::Matrix4& world_mat) {
      world_sphere->center = Ogre::Vector4{world_mat * Ogre::Vector4{sphere.center, 1.0F}}.xyz();
    }
  };

  using ColliderType = std::variant<Sphere>;
}  // namespace RenderHair::Collider
