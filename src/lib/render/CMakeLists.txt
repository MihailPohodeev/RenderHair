find_package(OGRE REQUIRED COMPONENTS Bites CONFIG)

add_library(RenderHair_Lib_Render
    "include/render_hair/render/exceptions.hpp"
    "include/render_hair/render/triangle.hpp"
    "include/render_hair/render/hair_root.hpp"
    "include/render_hair/render/mesh_conversion.hpp"
    "include/render_hair/render/hair_roots_generator.hpp"
    "include/render_hair/render/hair_model.hpp"
    "include/render_hair/render/hair_instance.hpp"
    "include/render_hair/render/hair_physics.hpp"
    "include/render_hair/render/verlet_cpu_hair_physics.hpp"
    "include/render_hair/render/verlet_gpu_hair_physics.hpp"
    "include/render_hair/render/object.hpp"

    "include/render_hair/render/collision/collider_manager.hpp"
    "include/render_hair/render/collision/colliders.hpp"

    "object.cpp"
    "hair_model.cpp"
    "hair_roots_generator.cpp"
    "mesh_conversion.cpp"
    "verlet_cpu_hair_physics.cpp"
    # "verlet_gpu_hair_physics.cpp"
)
RenderHair_setup_target(RenderHair_Lib_Render)
target_link_libraries(RenderHair_Lib_Render PUBLIC
    OgreBites
    RenderHair_Lib_Base
)
target_include_directories(RenderHair_Lib_Render
    PUBLIC
        "include"
)
