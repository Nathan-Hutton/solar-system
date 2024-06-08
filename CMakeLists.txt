cmake_minimum_required(VERSION 3.16)
project(SolarSystem)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++23")

# Enable all warnings
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
endif()

# Prefer GLVND for OpenGL if available
set(OpenGL_GL_PREFERENCE GLVND)

# Find the required packages
find_package(assimp REQUIRED)
find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)

# Add executable
file(GLOB SOURCES "src/*.cpp")
add_executable(main ${SOURCES})

# Link libraries
target_link_libraries(main ${ASSIMP_LIBRARIES} glfw GLEW ${OPENGL_LIBRARIES})
include_directories(${ASSIMP_INCLUDE_DIRS})
