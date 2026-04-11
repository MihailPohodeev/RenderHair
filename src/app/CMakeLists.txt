find_package(Qt6 REQUIRED COMPONENTS Widgets)
qt_standard_project_setup()

add_executable(RenderHair-app WIN32
    "main.cpp"
    "mainwindow.hpp"
    "mainwindow.cpp"
    "mainwindow.ui"
    "ico/app.rc") # windows icon
target_link_libraries(RenderHair-app PRIVATE Qt6::Widgets RenderHair)
RenderHair_setup_target(RenderHair-app)
