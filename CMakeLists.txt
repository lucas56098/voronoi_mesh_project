cmake_minimum_required(VERSION 3.0.0)
project(voronoi_mesh_project VERSION 0.1.0 LANGUAGES C CXX)

include(CTest)
enable_testing()

add_executable(voronoi_mesh_project main.cpp Point.cpp Halfplane.cpp VoronoiCell.cpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
