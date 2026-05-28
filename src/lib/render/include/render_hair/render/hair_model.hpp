#pragma once

#include <OgreHardwareVertexBuffer.h>
#include <OgreSceneManager.h>
#include <OgreSceneNode.h>

#include <cstdint>

namespace RenderHair {

  class HairModel {
    Ogre::Entity* entity_;
    Ogre::SceneNode* node_;
    Ogre::HardwareVertexBufferSharedPtr vertexBuffer_;

   public:
    struct Settings {
      int32_t points_per_strand{};
      float one_strand_length{};
    };

   private:
    Settings settings_;

   public:
    HairModel(Ogre::SceneNode& parent_node, Ogre::MeshPtr mesh, Ogre::SceneManager& scene_manager,
              Ogre::HardwareVertexBufferSharedPtr exаternal_buffer, const Settings& settings);
    ~HairModel();

    HairModel(const HairModel&) = delete;
    HairModel(HairModel&&) = delete;
    HairModel& operator=(const HairModel&) = delete;
    HairModel& operator=(HairModel&&) = delete;

    [[nodiscard]] Ogre::HardwareVertexBufferSharedPtr getVertexBuffer() const;
    [[nodiscard]] Ogre::SceneNode* getSceneNode() const;
  };

}  // namespace RenderHair
