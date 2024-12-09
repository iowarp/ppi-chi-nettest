cmake_minimum_required(VERSION 3.10)
project(hermes_shm LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)

#------------------------------------------------------------------------------
# Options
#------------------------------------------------------------------------------
option(HERMES_RPC_THALLIUM "Build tests which depend on thallium" ON)

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

# HermesShm
find_package(HermesShm CONFIG REQUIRED)
message(STATUS "found hermes_shm.h at ${HermesShm_INCLUDE_DIRS}")
include_directories(${HermesShm_INCLUDE_DIRS})
link_directories(${HermesShm_LIBRARY_DIRS})

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
include_directories(include)
add_executable(start_chi_net_test
        src/start_chi_net_test.cc)
target_link_libraries(start_chi_net_test ${HermesShm_LIBRARIES} thallium)

add_executable(stop_chi_net_test
        src/stop_chi_net_test.cc)
target_link_libraries(stop_chi_net_test ${HermesShm_LIBRARIES} thallium)


#------------------------------------------------------------------------------
# Install network tester
#------------------------------------------------------------------------------
install(
    TARGETS
        start_chi_net_test
        stop_chi_net_test
    DESTINATION
        ${CMAKE_INSTALL_PREFIX}/bin
)
