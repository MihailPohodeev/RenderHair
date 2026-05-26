find_package(glm REQUIRED)

add_library(RenderHair_Lib_Physics
    "include/render_hair/physics/exceptions.hpp"
    "include/render_hair/physics/hair_particle.hpp"
    # "include/render_hair/core/physics_config.hpp"
    # "include/render_hair/core/hair_unit.hpp"
    # "include/render_hair/core/connected_nodes.hpp"
    # "include/render_hair/core/hair_roots_allocation.hpp"
    # "include/render_hair/core/hair_auxiliary.hpp"
    # "include/render_hair/core/collision/base_colliders.hpp"

    "tmp.cpp"
    # "connected_nodes.cpp"
    # "hair_roots_allocation.cpp"
)
RenderHair_setup_target(RenderHair_Lib_Physics)
target_link_libraries(RenderHair_Lib_Physics
    PUBLIC
        glm::glm
        RenderHair_Lib_Base
)
target_include_directories(RenderHair_Lib_Physics PUBLIC "include")
