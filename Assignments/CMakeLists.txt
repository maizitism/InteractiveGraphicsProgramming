cmake_minimum_required(VERSION 3.21)
project(GLAssignments LANGUAGES CXX)
 
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
 
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(GLEW REQUIRED)

# Header only libraries
add_library(cy_headers INTERFACE)
target_include_directories(cy_headers INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/../lib)

# Add each assignment. Just add a new line when you start a new one.
add_subdirectory(Assignment01)
add_subdirectory(Assignment02)
# add_subdirectory(common)   # uncomment if you make shared code