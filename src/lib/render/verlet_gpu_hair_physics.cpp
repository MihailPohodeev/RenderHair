#include "render_hair/render/verlet_gpu_hair_physics.hpp"

#include <GL/gl3w.h>
#include <OgreGL3PlusHardwareBuffer.h>
#include <OgreGL3PlusRenderSystem.h>
#include <OgreGpuProgram.h>
#include <OgreHardwareBufferManager.h>
#include <OgreHighLevelGpuProgramManager.h>
#include <OgreLogManager.h>
#include <OgreRenderSystem.h>
#include <OgreRoot.h>

namespace RenderHair {

  VerletGPU_HairPhysics::VerletGPU_HairPhysics(Ogre::HardwareVertexBufferSharedPtr vertex_buffer,
                                               const HairPhysicsSettings& settings,
                                               const std::vector<Ogre::Vector4>& init_nodes)
      : settings_{settings},
        vertexBuffer_{vertex_buffer} {
    totalNodes_ = settings_.hairs_count * settings_.nodes_per_hair;
    initGPUBuffers(init_nodes);
    setupComputeShader();
  }

  void VerletGPU_HairPhysics::initGPUBuffers(const std::vector<Ogre::Vector4>& init_nodes) {
    if (totalNodes_ == 0) return;

    Ogre::HardwareBufferManager& bufferMgr = Ogre::HardwareBufferManager::getSingleton();

    previousPositionsBuffer_ =
        bufferMgr.createVertexBuffer(sizeof(float) * 4, totalNodes_, Ogre::HardwareBuffer::HBU_DYNAMIC);

    localRootsBuffer_ = bufferMgr.createVertexBuffer(sizeof(float) * 4, totalNodes_, Ogre::HardwareBuffer::HBU_DYNAMIC);

    if (!init_nodes.empty()) {
      vertexBuffer_->writeData(0, vertexBuffer_->getSizeInBytes(), init_nodes.data(), true);
      previousPositionsBuffer_->writeData(0, previousPositionsBuffer_->getSizeInBytes(), init_nodes.data(), true);
      localRootsBuffer_->writeData(0, localRootsBuffer_->getSizeInBytes(), init_nodes.data(), true);
    }
  }

  void VerletGPU_HairPhysics::setupComputeShader() {
    computeShader_ = Ogre::HighLevelGpuProgramManager::getSingleton().getByName("HairPhysicsCS");
    if (computeShader_ != nullptr) {
      Ogre::LogManager::getSingleton().logMessage("HairPhysicsCS shader was found", Ogre::LML_NORMAL);
      computeShader_->load();
      computeParams_ = computeShader_->getDefaultParameters();
    } else {
      Ogre::LogManager::getSingleton().logMessage("HairPhysicsCS shader not found", Ogre::LML_CRITICAL);
    }
  }

  void VerletGPU_HairPhysics::update(float delta_time, const Ogre::Matrix4& world_matrix) {
    if (totalNodes_ == 0 || delta_time <= 0.0F || computeParams_ == nullptr) { return; }

    const float hair_segment_length = settings_.one_hair_length / std::max(1, settings_.nodes_per_hair - 1);
    const Ogre::Vector4 gravity_vec(0.0F, -9.81F, 0.0F, 0.0F);

    computeParams_->setNamedConstant("world_matrix", world_matrix);
    computeParams_->setNamedConstant("gravity", gravity_vec);
    computeParams_->setNamedConstant("delta_time", delta_time);
    computeParams_->setNamedConstant("hair_segment_length", hair_segment_length);
    computeParams_->setNamedConstant("nodes_per_hair", static_cast<Ogre::uint32>(settings_.nodes_per_hair));
    computeParams_->setNamedConstant("total_hairs", static_cast<Ogre::uint32>(settings_.hairs_count));
    computeParams_->setNamedConstant("is_first_frame", static_cast<Ogre::uint32>(isFirstFrame_ ? 1 : 0));

    if (isFirstFrame_) { isFirstFrame_ = false; }

    Ogre::RenderSystem* renderSystem = Ogre::Root::getSingleton().getRenderSystem();
    renderSystem->bindGpuProgram(computeShader_.get());
    renderSystem->bindGpuProgramParameters(Ogre::GPT_COMPUTE_PROGRAM, computeParams_, Ogre::GPV_ALL);

    const int thread_groups_x = (settings_.hairs_count + 63) / 64;
    renderSystem->_dispatchCompute(Ogre::Vector3i{thread_groups_x, 1, 1});
  }
}  // namespace RenderHair
