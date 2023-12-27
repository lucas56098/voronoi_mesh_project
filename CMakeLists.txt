cmake_minimum_required(VERSION 3.5)
project(voronoi_mesh_project VERSION 0.1.0 LANGUAGES C CXX)

include(CTest)
enable_testing()

add_executable(vmp main.cpp Point.cpp Halfplane.cpp VoronoiCell.cpp VoronoiMesh.cpp)

# Set the name of the compiled program to "vmp"
set_target_properties(vmp PROPERTIES OUTPUT_NAME "vmp")

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
