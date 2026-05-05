add_executable(RenderHair-app WIN32
    "main.cpp"
    "plugins.cfg.in"
    "resources.cfg.in"
    "ico/app.rc" # windows icon
)
target_link_libraries(RenderHair-app
    PRIVATE
        RenderHair
        OgreBites
)
RenderHair_setup_target(RenderHair-app)

configure_file(plugins.cfg.in plugins.cfg @ONLY)
configure_file(resources.cfg.in resources.cfg @ONLY)
configure_file(${PROJECT_SOURCE_DIR}/assets/sphere_new.mesh sphere.mesh COPYONLY)
