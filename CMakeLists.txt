cmake_minimum_required(VERSION 2.6)

# Name of project
project(GeomProc)

# Set header files for library
set(HDRS
    ${CMAKE_SOURCE_DIR}/GeomProcLib/include/graph_dist.h
    ${CMAKE_SOURCE_DIR}/GeomProcLib/include/mesh.h
    ${CMAKE_SOURCE_DIR}/GeomProcLib/include/model_loading.h
    ${CMAKE_SOURCE_DIR}/GeomProcLib/include/utils.h
)
include_directories(${CMAKE_SOURCE_DIR}/GeomProcLib/include)
include_directories(${CMAKE_SOURCE_DIR})

# GeomProc library
add_subdirectory(GeomProcLib/src)

# Demo executables
add_executable(graph_dist_demo ${HDRS} graph_dist_demo.cpp)
add_executable(graph_dist_faces_demo ${HDRS} graph_dist_faces_demo.cpp)

target_link_libraries(graph_dist_demo LINK_PUBLIC GeomProcLib)
target_link_libraries(graph_dist_faces_demo LINK_PUBLIC GeomProcLib)
