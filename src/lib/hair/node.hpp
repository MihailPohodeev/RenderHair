#ifndef RENDER_HAIR_NODE_HPP
#define RENDER_HAIR_NODE_HPP

#include <glm/glm.hpp>

namespace RenderHair {
struct VerletNode {
  glm::vec3 currentPosition;
  glm::vec3 previousPosition;
  bool isPinned;
};
}  // namespace RenderHair

#endif
