#pragma once

#include <OgreHardwareBuffer.h>
#include <OgreMatrix4.h>
#include <OgreVector3.h>
#include <OgreVector4.h>

#include "render_hair/render/hair_physics.hpp"

namespace RenderHair {

  class VerletGPU_HairPhysics {
    Ogre::HardwareVertexBufferSharedPtr vertexBuffer_;
    Ogre::HardwareVertexBufferSharedPtr previousPositionsBuffer_;
    Ogre::HardwareVertexBufferSharedPtr localRootsBuffer_;
    Ogre::UavBuffer

        HairPhysicsSettings settings_;
    size_t totalNodes_{0};

    Ogre::HighLevelGpuProgramPtr computeShader_;
    Ogre::GpuProgramParametersSharedPtr computeParams_;
    bool isFirstFrame_{true};

   public:
    VerletGPU_HairPhysics(Ogre::HardwareVertexBufferSharedPtr vertex_buffer, const HairPhysicsSettings& settings,
                          const std::vector<Ogre::Vector4>& init_nodes = {});

    void update(float delta_time, const Ogre::Matrix4& world_matrix);

   private:
    void initGPUBuffers(const std::vector<Ogre::Vector4>& init_nodes);
    void setupComputeShader();
  };

  static_assert(HairPhysicsConcept<VerletGPU_HairPhysics>,
                "`VerletCPU_HairPhysics` class doesn't satisfy `HairPhysicsConcept`.");

}  // namespace RenderHair
