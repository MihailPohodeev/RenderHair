find_package(OGRE REQUIRED COMPONENTS Bites CONFIG)

add_library(RenderHair_Lib_Render
    "include/render_hair/render/exceptions.hpp"
    "include/render_hair/render/triangle.hpp"
    "include/render_hair/render/hair_root.hpp"
    "include/render_hair/render/mesh_conversion.hpp"
    "include/render_hair/render/hair_generator.hpp"
    "include/render_hair/render/hair_model.hpp"
    "include/render_hair/render/hair_renderable.hpp"

    "hair_model.cpp"
    "hair_renderable.cpp"
    "hair_generator.cpp"
    "mesh_conversion.cpp"
)
RenderHair_setup_target(RenderHair_Lib_Render)
target_link_libraries(RenderHair_Lib_Render PUBLIC
    OgreBites
    RenderHair_Lib_Base
    RenderHair_Lib_Physics
)
target_include_directories(RenderHair_Lib_Render PUBLIC "include")
