find_package(glm REQUIRED)

add_library(RenderHair
    "physics_config.hpp"
    "hair_unit.hpp"
    "connected_nodes.hpp"
    "connected_nodes.cpp"
    "hair_roots_allocation.hpp"
    "hair_roots_allocation.cpp"
    "exceptions.hpp"
    "hair_auxiliary.hpp"

    "collision/base_colliders.hpp"
)
RenderHair_setup_target(RenderHair)
target_link_libraries(RenderHair
    PUBLIC
        glm::glm
)
target_include_directories(RenderHair PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
