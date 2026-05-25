find_package(glm REQUIRED)

add_library(RenderHair_Lib_Core
    "include/render_hair/core/exceptions.hpp"
    "include/render_hair/core/triangle.hpp"
    "include/render_hair/core/hair_root.hpp"
    # "include/render_hair/core/physics_config.hpp"
    # "include/render_hair/core/hair_unit.hpp"
    # "include/render_hair/core/connected_nodes.hpp"
    # "include/render_hair/core/hair_roots_allocation.hpp"
    # "include/render_hair/core/hair_auxiliary.hpp"
    # "include/render_hair/core/collision/base_colliders.hpp"

    # "connected_nodes.cpp"
    "hair_roots_allocation.cpp"
)
RenderHair_setup_target(RenderHair_Lib_Core)
target_link_libraries(RenderHair_Lib_Core
    PUBLIC
        glm::glm
        RenderHair_Lib_Base
)
target_include_directories(RenderHair_Lib_Core PUBLIC "include")
