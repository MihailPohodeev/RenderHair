add_executable(RenderHair-app WIN32
    "main.cpp"
    "plugins.cfg.in"
    "resources.cfg.in"
)
target_link_libraries(RenderHair-app PRIVATE RenderHair_Lib_Render)
RenderHair_setup_target(RenderHair-app)

add_library(RenderHair-Assets INTERFACE
    "shaders/HairSphere.program"
    "shaders/simple.vert"
    "shaders/simple.frag"
    "shaders/ComputeShaders.program"
    "shaders/VerletPhysics.glsl"

    "assets/materials/HairSphere.material"

    "assets/textures/HairSphere_hair_distribution_map.png"
    "assets/textures/RustySteel.png"
)

configure_file(plugins.cfg.in plugins.cfg @ONLY)
configure_file(resources.cfg.in resources.cfg @ONLY)
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/assets" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/shaders" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
