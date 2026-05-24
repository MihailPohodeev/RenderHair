#include "catch2/catch_test_macros.hpp"
#include "hair_roots_allocation.hpp"

TEST_CASE("One simple triangle") {
  // std::vector<RenderHair::Triangle> triangles;
  // triangles.emplace_back(RenderHair::Triangle{
  //     {glm::vec3{0.0F, 0.0F, 0.0F}, glm::vec3{1.0F, 0.0F, 0.0F},
  //      glm::vec3{0.0F, 1.0F, 0.0F}},
  //     {},
  //     {glm::vec2{0.0F, 0.0F}, glm::vec2{1.0F, 0.0F},
  //     glm::vec2{1.0F, 1.0F}}});

  // auto hair_roots = RenderHair::HairRootsAllocation::allocate(triangles);

  CHECK(true);
}
