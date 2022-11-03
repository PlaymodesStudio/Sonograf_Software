cmake_minimum_required(VERSION 3.11) # FetchContent is available in 3.11+
project(Sonograf)

# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set (CMAKE_CXX_STANDARD 17)
add_definitions(-D COMPILE_WITHOUT_OPENFRAMEWORKS)

# Dependencies
find_package(raylib 4.2.0 QUIET) # QUIET or REQUIRED
if (NOT raylib_FOUND) # If there's none, fetch and build raylib
  include(FetchContent)
  FetchContent_Declare(
    raylib
    URL https://github.com/raysan5/raylib/archive/refs/tags/4.2.0.tar.gz
  )
  FetchContent_GetProperties(raylib)
  if (NOT raylib_POPULATED) # Have we downloaded raylib yet?
    set(FETCHCONTENT_QUIET NO)
    FetchContent_Populate(raylib)
    set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples
    if(NOT APPLE)
      set(PLATFORM DRM)
    endif()
    add_subdirectory(${raylib_SOURCE_DIR} ${raylib_BINARY_DIR})
  endif()
endif()
find_package(OpenCV REQUIRED)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake/")
find_package(oscpack REQUIRED)

# Our Project
include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${OSCPACK_INCLUDE_DIRS})

add_subdirectory(src)


