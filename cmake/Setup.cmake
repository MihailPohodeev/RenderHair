option(RENDERHAIR_ENABLE_WARNINGS          "more warnings and werror"  ${RENDERHAIR_ENABLE_DEVELOPER_MODE})
option(RENDERHAIR_ENABLE_TEST_COVERAGE     "test coverage"             ${RENDERHAIR_ENABLE_DEVELOPER_MODE})
option(RENDERHAIR_ENABLE_STATIC_ANALYSIS   "clang-tidy, cppcheck"      ${RENDERHAIR_ENABLE_DEVELOPER_MODE})
option(RENDERHAIR_ENABLE_DYNAMIC_ANALYSIS  "available sanitizers"      ${RENDERHAIR_ENABLE_DEVELOPER_MODE})

include("cmake/Cache.cmake")
include("cmake/Warnings.cmake")
include("cmake/Hardening.cmake")
include("cmake/NiceOutput.cmake")
include("cmake/TestCoverage.cmake")
include("cmake/StaticAnalysis.cmake")
include("cmake/DynamicAnalysis.cmake")
include("cmake/InterproceduralOptimizations.cmake")

function(RenderHair_setup_target TARGET_NAME)

    set_target_properties(
        ${TARGET_NAME} PROPERTIES
        CXX_STANDARD            23
        CXX_STANDARD_REQUIRED   ON
        EXPORT_COMPILE_COMMANDS ON
        CXX_EXTENSIONS          OFF)
    
    __RenderHair_target_enable_cache(${TARGET_NAME})
    __RenderHair_target_enable_warnings(${TARGET_NAME})
    __RenderHair_target_enable_hardening(${TARGET_NAME})
    __RenderHair_target_enable_nice_output(${TARGET_NAME})
    __RenderHair_target_enable_test_coverage(${TARGET_NAME})
    __RenderHair_target_enable_static_analysis(${TARGET_NAME})
    __RenderHair_target_enable_dynamic_analysis(${TARGET_NAME})
    __RenderHair_target_enable_interprocedural_optimizations(${TARGET_NAME})

endfunction()
