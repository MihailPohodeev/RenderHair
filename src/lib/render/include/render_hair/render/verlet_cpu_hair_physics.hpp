#pragma once

#include <OgreMatrix4.h>
#include <OgreVector3.h>
#include <OgreVector4.h>

#include <vector>

#include "render_hair/render/hair_physics.hpp"

namespace RenderHair {

  class VerletCPU_HairPhysics {
    Ogre::HardwareVertexBufferSharedPtr vertexBuffer_;

    std::vector<Ogre::Vector3> localRootsCache_;
    std::vector<Ogre::Vector4> currentPositionsCache_;
    std::vector<Ogre::Vector3> previousPositions_;
    bool isFirstFrame_{true};

    HairPhysicsSettings settings_;
    size_t totalNodes_;
    size_t hairCount_;

   public:
    VerletCPU_HairPhysics(Ogre::HardwareVertexBufferSharedPtr vertex_buffer, const HairPhysicsSettings& settings,
                          const std::vector<Ogre::Vector4>& init_nodes = {});

    void update(float delta_time, const Ogre::Matrix4& world_matrix);

   private:
    static float extract_axisX_scale_coefficient(const Ogre::Matrix4& matrix);
    void relaxationConstraints(float target_length, int constraint_iterations, float stiffness);
    void resolveCollisions();
  };

  static_assert(HairPhysicsConcept<VerletCPU_HairPhysics>,
                "`VerletCPU_HairPhysics` class doesn't satisfy `HairPhysicsConcept`.");

}  // namespace RenderHair
