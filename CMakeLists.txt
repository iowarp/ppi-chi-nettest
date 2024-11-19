cmake_minimum_required(VERSION 3.10)
project(hermes_shm LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)

#------------------------------------------------------------------------------
# Global variables
#------------------------------------------------------------------------------
set(HERMES_SHM_VERSION_MAJOR 1)
set(HERMES_SHM_VERSION_MINOR 1)
set(HERMES_SHM_VERSION_PATCH 0)
set(HERMES_SHM_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${HERMES_SHM_ROOT}/include)

#------------------------------------------------------------------------------
# Options
#------------------------------------------------------------------------------
option(HERMES_RPC_THALLIUM "Build tests which depend on thallium" ON)

if (HERMES_RPC_THALLIUM)
    add_compile_definitions(HERMES_RPC_THALLIUM)
endif()

#------------------------------------------------------------------------------
# Setup CMake Environment
#------------------------------------------------------------------------------
if(NOT HERMES_EXTERNALLY_CONFIGURED)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY
            ${PROJECT_BINARY_DIR}/bin CACHE PATH "Single Directory for all Executables.")
    set(EXECUTABLE_OUTPUT_PATH ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY
            ${PROJECT_BINARY_DIR}/bin CACHE PATH "Single Directory for all Libraries")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY
            ${PROJECT_BINARY_DIR}/bin CACHE PATH "Single Directory for all static libraries.")
endif()

#------------------------------------------------------------------------------
# Setup install and output Directories
#------------------------------------------------------------------------------
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

#------------------------------------------------------------------------------
# CMake Modules
#------------------------------------------------------------------------------
if (NOT IS_HERMES_MAIN)
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/CMake)
    message(${CMAKE_MODULE_PATH})
endif()

#------------------------------------------------------------------------------
# Optimization
#------------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 17)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message("IN DEBUG MODE")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0")
else()
    message("IN RELEASE MODE")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O3")
endif()
if(HERMES_CXX_PROFILE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
    message("CXX PROFILER IS ON")
endif()
#function(make_gprof exec_name exec_dir)
#    add_custom_target(
#      ${exec_name}_gprof
#      COMMAND gprof -b -A -p -q ${exec_dir}/${exec_name} gmon.out)
#endfunction()

#------------------------------------------------------------------------------
# External libraries
#------------------------------------------------------------------------------
# This is for compatability with SPACK
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# thallium
if(HERMES_RPC_THALLIUM)
    find_package(thallium CONFIG REQUIRED)
    if(thallium_FOUND)
        message(STATUS "found thallium at ${thallium_DIR}")
    endif()
endif()

#------------------------------------------------------------------------------
# Compile the network tester
#------------------------------------------------------------------------------
add_executable(test_networks
        test_networks.cc)
target_link_libraries(test_networks )

#------------------------------------------------------------------------------
# Install network tester
#------------------------------------------------------------------------------
install(
    TARGETS
        test_networks
    DESTINATION
        ${CMAKE_INSTALL_PREFIX}/bin
)
