cmake_minimum_required(VERSION 3.10)
project(chi_netping LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_WITH_INSTALL_RPATH ON)
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# ------------------------------------------------------------------------------
# Options
# ------------------------------------------------------------------------------'
option(CHI_ENABLE_CMAKE_DOTENV "Load environment variables from .env.cmake" OFF)

set(HSHM_RPC_THALLIUM ON)
set(BUILD_HSHM_TESTS OFF)

# ------------------------------------------------------------------------------
# DOTENV
# ------------------------------------------------------------------------------
if(CHI_ENABLE_CMAKE_DOTENV)
    if(EXISTS ${CMAKE_SOURCE_DIR}/.env.cmake)
        include(.env.cmake)
    endif()
endif()

# ------------------------------------------------------------------------------
# Setup CMake Environment
# ------------------------------------------------------------------------------
if(NOT HERMES_EXTERNALLY_CONFIGURED)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY
        ${PROJECT_BINARY_DIR}/bin CACHE PATH "Single Directory for all Executables.")
    set(EXECUTABLE_OUTPUT_PATH ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY
        ${PROJECT_BINARY_DIR}/bin CACHE PATH "Single Directory for all Libraries")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY
        ${PROJECT_BINARY_DIR}/bin CACHE PATH "Single Directory for all static libraries.")
endif()

# ------------------------------------------------------------------------------
# Setup install and output Directories
# ------------------------------------------------------------------------------
if(NOT HERMES_INSTALL_BIN_DIR)
    set(HERMES_INSTALL_BIN_DIR ${CMAKE_INSTALL_PREFIX}/bin)
endif()

if(NOT HERMES_INSTALL_LIB_DIR)
    set(HERMES_INSTALL_LIB_DIR ${CMAKE_INSTALL_PREFIX}/lib)
endif()

if(NOT HERMES_INSTALL_INCLUDE_DIR)
    set(HERMES_INSTALL_INCLUDE_DIR ${CMAKE_INSTALL_PREFIX}/include)
endif()

if(NOT HERMES_INSTALL_DATA_DIR)
    set(HERMES_INSTALL_DATA_DIR ${CMAKE_INSTALL_PREFIX}/share)
endif()

# ------------------------------------------------------------------------------
# Optimization
# ------------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 17)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message("IN DEBUG MODE")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0")
    add_compile_definitions(HERMES_LOG_VERBOSITY=10)
else()
    message("IN RELEASE MODE")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O3")
    add_compile_definitions(HERMES_LOG_VERBOSITY=1)
endif()

if(HERMES_CXX_PROFILE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
    message("CXX PROFILER IS ON")
endif()

# function(make_gprof exec_name exec_dir)
# add_custom_target(
# ${exec_name}_gprof
# COMMAND gprof -b -A -p -q ${exec_dir}/${exec_name} gmon.out)
# endfunction()

# ------------------------------------------------------------------------------
# External libraries
# ------------------------------------------------------------------------------
# This is for compatability with SPACK
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_WITH_INSTALL_RPATH ON)
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# HermesShm
# find_package(HermesShm CONFIG REQUIRED)
# message("RPATH: ${CMAKE_INSTALL_RPATH}")
add_subdirectory(cte-hermes-shm)
include_directories(cte-hermes-shm/include)

# Yaml-cpp
find_package(yaml-cpp REQUIRED)

# thallium
if(HSHM_RPC_THALLIUM)
    find_package(thallium CONFIG REQUIRED)

    if(thallium_FOUND)
        message(STATUS "found thallium at ${thallium_DIR}")
    endif()
endif()

# pkgconifg
find_package(PkgConfig REQUIRED)

if(PkgConfig)
    message(STATUS "found pkg config")
endif()

# libfabric
pkg_check_modules(libfabric REQUIRED libfabric)

# ------------------------------------------------------------------------------
# Compile the network tester
# ------------------------------------------------------------------------------
include_directories(include)
add_executable(chi_net_bench
    src/chi_net_bench.cc)
target_link_libraries(chi_net_bench thallium cxx host_deps)

add_executable(chi_net_ping
    src/chi_net_ping.cc)
target_link_libraries(chi_net_ping thallium cxx host_deps)

# add_executable(fullping src/fullping.cc)
# target_link_libraries(fullping ${libfabric_LIBRARIES} hshm::cxx)
# target_include_directories(fullping PUBLIC ${libfabric_INCLUDE_DIRS})
# target_link_directories(fullping PUBLIC ${libfabric_LIBRARY_DIRS}) # ------------------------------------------------------------------------------

# Install network tester
# ------------------------------------------------------------------------------
install(
    TARGETS

    # fullping
    chi_net_bench
    chi_net_ping
    DESTINATION
    ${CMAKE_INSTALL_PREFIX}/bin
)
