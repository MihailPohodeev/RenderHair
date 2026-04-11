cmake_minimum_required(VERSION 3.28)

project(RenderHair)

include("cmake/Setup.cmake")

option(RENDERHAIR_BUILD_TESTS "build tests" ${RENDERHAIR_ENABLE_DEVELOPER_MODE})

add_subdirectory("deps" EXCLUDE_FROM_ALL)
add_subdirectory("src/app")
add_subdirectory("src/lib")

if (RENDERHAIR_BUILD_TESTS)
    enable_testing()
    add_subdirectory("tests")
endif()
