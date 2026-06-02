add_executable(RenderHair-app WIN32
    "main.cpp"
    "plugins.cfg.in"
    "resources.cfg.in"
)
target_link_libraries(RenderHair-app PRIVATE RenderHair_Lib_Render)
target_include_directories(RenderHair-app PRIVATE "/Libraries/ogre/linux_gcc_amd64/Release/include/OGRE/RenderSystems/GL3Plus")
RenderHair_setup_target(RenderHair-app)

add_library(RenderHair-Assets INTERFACE
    "shaders/HairVertex.glsl"
    "shaders/HairFragment.glsl"
    "shaders/HairGeometry.glsl"
    "shaders/HairPhysics.glsl"
    "shaders/ComputeShaders.program"

    "assets/materials/Hair.material"

    "assets/textures/HairSphere_hair_allocation_map.png"
    "assets/textures/RustySteel.png"
)

configure_file(plugins.cfg.in plugins.cfg @ONLY)
configure_file(resources.cfg.in resources.cfg @ONLY)
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/assets" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/shaders" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
