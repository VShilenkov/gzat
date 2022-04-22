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

if(WITH_COVERAGE)
    include(CodeCoverage)
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

foreach(compiler_flag -Wall -Wextra -Wpedantic)
    check_cxx_compiler_flag(${compiler_flag} supports${compiler_flag})
    if(supports${compiler_flag})
        target_compile_options(${PROJECT_NAME} 
            PRIVATE 
                ${compiler_flag}
        )
    endif()
endforeach()

target_compile_options(${PROJECT_NAME} 
    PRIVATE 
        -Werror
)

set(gzat_PUBLIC_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/include/gzat_parser.hpp)

set_target_properties(gzat
    PROPERTIES
        PUBLIC_HEADER ${gzat_PUBLIC_HEADER}
)

# Package configuration --------------------------------------------------------

configure_package_config_file(
        cmake/templates/${PACKAGE_NAME}Config.cmake.in
        ${PACKAGE_NAME}Config.cmake
    INSTALL_DESTINATION ${PACKAGE_INSTALL_CMAKE_DIR}
    NO_SET_AND_CHECK_MACRO
)

write_basic_package_version_file(${PROJECT_NAME}ConfigVersion.cmake
    COMPATIBILITY AnyNewerVersion
)

# pkgconfig configuration ------------------------------------------------------
configure_file("cmake/templates/${PACKAGE_NAME}.pc.in"
               "${PACKAGE_NAME}.pc" 
    @ONLY
)

# Static Code analysis ---------------------------------------------------------
if(WITH_CLANG_TIDY)
    target_setup_clang_tidy(${PROJECT_NAME})
endif()

# Testing ----------------------------------------------------------------------
if(WITH_UNIT_TEST)

    if(WITH_COVERAGE)
        target_setup_coverage(${PROJECT_NAME})
    endif()

    include(CTest)
    enable_testing()
    add_subdirectory(test)
endif()


# Feature summary --------------------------------------------------------------
feature_summary(WHAT ALL
    DESCRIPTION "-- [${PROJECT_NAME} summary] ---------------------------------"
)

# Installations ----------------------------------------------------------------

## Targets installation
install(TARGETS                 ${PROJECT_NAME}
    EXPORT                      ${PROJECT_NAME}Targets
    LIBRARY         DESTINATION ${PACKAGE_INSTALL_LIBRARY_DIR}
    PUBLIC_HEADER   DESTINATION ${PACKAGE_INSTALL_INCLUDE_DIR}
    COMPONENT                   ${PROJECT_NAME}
)

## CMake's configurations
install(EXPORT ${PROJECT_NAME}Targets
    NAMESPACE   ${PACKAGE_NAMESPACE}::
    DESTINATION ${PACKAGE_INSTALL_CMAKE_DIR}
)


## Package configurations
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}Config.cmake 
              ${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}ConfigVersion.cmake
    DESTINATION ${PACKAGE_INSTALL_CMAKE_DIR}
)

## pkgconfig configurations ----------------------------------------------------
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}.pc"
    DESTINATION ${PACKAGE_INSTALL_LIBRARY_DIR}/pkgconfig
)

# Packaging --------------------------------------------------------------------
list(APPEND CPACK_GENERATOR DEB)
find_program(rpmbuild_COMMAND rpmbuild)
if(rpmbuild_COMMAND)
    list(APPEND CPACK_GENERATOR RPM)
endif()

set(CPACK_PACKAGE_NAME                "${PACKAGE_NAME}")
set(CPACK_PACKAGE_VENDOR              "Guanyu Zhou")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${PROJECT_DESCRIPTION}")

set(CPACK_PACKAGE_VERSION       "${PROJECT_VERSION}")
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")

set(CPACK_PACKAGE_INSTALL_DIRECTORY "${PACKAGE_NAME}")

set(CPACK_PACKAGING_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_SOURCE_DIR}/LICENSE)

set(CPACK_DEBIAN_PACKAGE_NAME       "${CPACK_PACKAGE_NAME}")
set(CPACK_DEBIAN_PACKAGE_HOMEPAGE   "${PROJECT_HOMEPAGE_URL}")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "${CPACK_PACKAGE_VENDOR}")
set(CPACK_DEBIAN_PACKAGE_VERSION    "${CPACK_PACKAGE_VERSION}")
set(CPACK_DEBIAN_PACKAGE_PROVIDES   "${CPACK_PACKAGE_NAME}")
set(CPACK_DEBIAN_PACKAGE_DEPENDS    "libc6, libstdc++6, cmake (>= 3.12)")
set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE i386)
find_program(DPKG_COMMAND dpkg)
if(DPKG_COMMAND)
    execute_process(
        COMMAND "${DPKG_COMMAND}" --print-architecture
        OUTPUT_VARIABLE CPACK_DEBIAN_PACKAGE_ARCHITECTURE
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
endif()
set(CPACK_DEBIAN_FILE_NAME 
    "${CPACK_DEBIAN_PACKAGE_NAME}_${CPACK_DEBIAN_PACKAGE_VERSION}_${CPACK_DEBIAN_PACKAGE_ARCHITECTURE}.deb"
)

set(CPACK_RPM_PACKAGE_NAME       "${CPACK_PACKAGE_NAME}")
set(CPACK_RPM_PACKAGE_HOMEPAGE   "${PROJECT_HOMEPAGE_URL}")
set(CPACK_RPM_PACKAGE_MAINTAINER "${CPACK_PACKAGE_VENDOR}")
set(CPACK_RPM_PACKAGE_VERSION    "${CPACK_PACKAGE_VERSION}")
if(NOT CPACK_RPM_PACKAGE_ARCHITECTURE)
    set(CPACK_RPM_PACKAGE_ARCHITECTURE "${CMAKE_SYSTEM_PROCESSOR}")
endif()
set(CPACK_RPM_FILE_NAME 
    "${CPACK_RPM_PACKAGE_NAME}_${CPACK_RPM_PACKAGE_VERSION}_${CPACK_RPM_PACKAGE_ARCHITECTURE}.rpm"
)

# This must always be after all CPACK\_\* variables are defined
include(CPack)
