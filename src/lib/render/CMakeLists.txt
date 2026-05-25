find_package(OGRE REQUIRED COMPONENTS Bites CONFIG)

add_library(RenderHair_Lib_Render
    "include/render_hair/render/exceptions.hpp"
    "include/render_hair/render/mesh_conversion.hpp"

    "mesh_conversion.cpp"
)
RenderHair_setup_target(RenderHair_Lib_Render)
target_link_libraries(RenderHair_Lib_Render PUBLIC
    OgreBites
    RenderHair_Lib_Core
)
target_include_directories(RenderHair_Lib_Render PUBLIC "include")
