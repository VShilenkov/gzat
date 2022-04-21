# Requirements to the cmake ----------------------------------------------------
cmake_minimum_required(VERSION 3.12)

# Project Declaration ----------------------------------------------------------

project(gzat
    VERSION         0.1.0
    DESCRIPTION     "Portable AT command parsing library in C++ language."
    HOMEPAGE_URL    "https://github.com/gzhouct/gzat"
    LANGUAGES       CXX
)

# Project wide settings --------------------------------------------------------
set(CMAKE_CXX_STANDARD              11)
set(CMAKE_CXX_STANDARD_REQUIRED     true)
set(CMAKE_CXX_EXTENSIONS            false)
set(CMAKE_POSITION_INDEPENDENT_CODE true)

# CMake extensions -------------------------------------------------------------
add_subdirectory(cmake)

# Includes ---------------------------------------------------------------------
## cmake native modules
include(CheckCXXCompilerFlag)
include(CMakePackageConfigHelpers)
include(FeatureSummary)
include(GNUInstallDirs)

## local modules
include(CommonOptions)

if(WITH_INCLUDE_WHAT_YOU_USE)
    include(UseIncludeWhatYouUse)
endif()

if(WITH_CLANG_TIDY)
    include(UseClangTidy)
endif()

# Package setting -------------------------------------------------------------

set(PACKAGE_NAME                ${PROJECT_NAME})
set(PACKAGE_NAMESPACE           ${PROJECT_NAME})
set(PACKAGE_INSTALL_CMAKE_DIR   ${CMAKE_INSTALL_LIBDIR}/cmake/${PACKAGE_NAME})
set(PACKAGE_INSTALL_INCLUDE_DIR ${CMAKE_INSTALL_INCLUDEDIR}/${PACKAGE_NAME})
set(PACKAGE_INSTALL_LIBRARY_DIR ${CMAKE_INSTALL_LIBDIR})


# Target Declaration -----------------------------------------------------------
add_library(${PROJECT_NAME} STATIC)
add_library(${PACKAGE_NAMESPACE}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

# Dependency resolving ---------------------------------------------------------

# Target Definition ------------------------------------------------------------

target_sources(${PROJECT_NAME}
    PRIVATE
        source/gzat_parser.cpp
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${PACKAGE_INSTALL_INCLUDE_DIR}>
)

foreach(compiler_flag -Wall -Werror -Wextra -Wpedantic)
    check_cxx_compiler_flag(${compiler_flag} supports${compiler_flag})
    if(supports${compiler_flag})
        target_compile_options(${PROJECT_NAME} PRIVATE ${compiler_flag})
    endif()
endforeach()

if(WITH_CLANG_TIDY)
    target_setup_clang_tidy(${PROJECT_NAME})
endif()

# Testing ----------------------------------------------------------------------
if(WITH_UNIT_TEST)
    include(CTest)
    enable_testing()
    add_subdirectory(test)
endif()


# Feature summary --------------------------------------------------------------
feature_summary(WHAT ALL
    DESCRIPTION "-- [${PROJECT_NAME} summary] ---------------------------------"
)