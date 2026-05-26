#pragma once

#include <Ogre.h>

#include "render_hair/render/hair_root.hpp"

namespace RenderHair::Render {

  class HairModel {
    Ogre::SceneNode* node_;
    Ogre::Entity* entity_;
    Ogre::HardwareVertexBufferSharedPtr vertexBuffer_;

   public:
    HairModel(Ogre::SceneManager& scene_manager, Ogre::SceneNode& scene_node, Ogre::MeshPtr mesh,
              std::span<const HairRoot> roots);
    ~HairModel();

    Ogre::HardwareVertexBufferSharedPtr getVertexBuffer() const;
  };

}  // namespace RenderHair::Render
