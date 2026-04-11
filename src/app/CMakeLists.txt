add_executable(RenderHair-app WIN32
    "main.cpp"
    "ico/app.rc") # windows icon
target_link_libraries(RenderHair-app PRIVATE RenderHair)
RenderHair_setup_target(RenderHair-app)
