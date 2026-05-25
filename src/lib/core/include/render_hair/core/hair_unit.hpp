#ifndef RENDER_HAIR_HAIR_UNIT_HPP
#define RENDER_HAIR_HAIR_UNIT_HPP

#include <concepts>
#include <glm/glm.hpp>

namespace RenderHair {
template <class HairUnit>
concept isHairUnit = requires(const HairUnit& hair_unit, int index) {
  { hair_unit.getNodesCount() } -> std::same_as<int>;
  { hair_unit.getNodePosition(index) } -> std::same_as<glm::vec3>;
};
}  // namespace RenderHair

#endif
