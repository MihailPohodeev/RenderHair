#pragma once

#include <OgreEntity.h>
#include <OgreSceneManager.h>

namespace RenderHair {
  class Object {
    Ogre::SceneNode* node_{nullptr};
    std::vector<Ogre::Entity*> entities_;

   public:
    Object();

    void addEntity(Ogre::Entity* new_entity);
    void setNode(Ogre::SceneNode* node);
    Ogre::SceneNode* getNode() const;
  };
}  // namespace RenderHair
