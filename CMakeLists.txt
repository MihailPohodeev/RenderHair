cmake_minimum_required(VERSION 3.28)
project(RenderHair VERSION 0.1.0)

include("cmake/Setup.cmake")

set(CMAKE_LINK_LIBRARIES_ONLY_TARGETS ON)

option(RENDERHAIR_BUILD_TESTS "build tests" ${RENDERHAIR_ENABLE_DEVELOPER_MODE})
option(BUILD_SHARED_LIBS "type of libraries" OFF)

find_package(OGRE REQUIRED COMPONENTS Bites CONFIG)

add_subdirectory("deps" EXCLUDE_FROM_ALL)
add_subdirectory("src/app")
add_subdirectory("src/lib")

if (RENDERHAIR_BUILD_TESTS)
    enable_testing()
    add_subdirectory("tests")
endif()
