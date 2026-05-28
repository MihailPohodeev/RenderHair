#include "render_hair/render/object.hpp"

namespace RenderHair {

  Object::Object() = default;

  void Object::addEntity(Ogre::Entity* new_entity) {
    entities_.push_back(new_entity);
    if (node_ == nullptr) { return; }
    node_->attachObject(new_entity);
  }

  void Object::setNode(Ogre::SceneNode* node) {
    node_ = node;
    for (auto* entity : entities_) { node->attachObject(entity); }
  }

  Ogre::SceneNode* Object::getNode() const { return node_; }

}  // namespace RenderHair
