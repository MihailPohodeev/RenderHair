#pragma once

#include <OgreHardwareVertexBuffer.h>
#include <OgreVector4.h>

namespace RenderHair {
  struct HairPhysicsSettings {
    int32_t hairs_count{};
    int32_t nodes_per_hair{};
    float one_hair_length{};
  };

  template <typename PhysicsType>
  concept HairPhysicsConcept =
      requires(PhysicsType physics, float delta_time, const Ogre::Matrix4& mat, HairPhysicsSettings settings,
               Ogre::HardwareVertexBufferSharedPtr write_buffer) {
        PhysicsType{write_buffer, settings};
        { physics.update(delta_time, mat) } -> std::same_as<void>;
      };
}  // namespace RenderHair
