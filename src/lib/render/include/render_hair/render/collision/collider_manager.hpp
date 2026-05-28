#pragma once

#include <list>
#include <render_hair/render/collision/colliders.hpp>

namespace RenderHair {
  class ColliderManager {
    std::list<Collider::ColliderType> colliders_;

    ColliderManager() = default;

   public:
    static ColliderManager& getInstance() {
      static ColliderManager instance;
      return instance;
    }

    Collider::SphereCollider registerSphereCollider(const Collider::Sphere& collider) {
      auto iterator = colliders_.insert(colliders_.end(), collider);
      auto* address = std::get_if<Collider::Sphere>(std::addressof(*iterator));
      return {.sphere = collider, .world_sphere = address};
    }

    [[nodiscard]] std::list<Collider::ColliderType> getColliders() { return colliders_; }
  };
}  // namespace RenderHair
