# Specify project files: header files and source files
 
set(SRCS
    graph_dist.cpp
    mesh.cpp
    utils.cpp
)

# Add library
add_library(GeomProcLib ${HDRS} ${SRCS})
target_include_directories(GeomProcLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
