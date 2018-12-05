cmake_minimum_required(VERSION 3.12)

project(ITR)
set(ITR_MAJOR_VERSION 0)
set(ITR_MINOR_VERSION 5)

set(default_build_type "Release")
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
  set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE STRING "Choose the type of build." FORCE)
endif()

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS_DEBUG "-Wall -O0 -g -pthread")
set(CMAKE_CXX_FLAGS_RELEASE "-Wall -O3 -pthread")

add_subdirectory(src)
add_subdirectory(include)
add_subdirectory(demo)
