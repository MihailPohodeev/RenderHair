add_library(RenderHair
    "RenderHair.hpp"
    "RenderHair.cpp")
RenderHair_setup_target(RenderHair)

target_include_directories(RenderHair PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
