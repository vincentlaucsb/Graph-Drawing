cmake_minimum_required(VERSION 3.9)
set(CMAKE_CXX_STANDARD 14)
project(Force_Directed_Drawing)
include_directories(
	${CMAKE_SOURCE_DIR}/lib/svg/src/
	${CMAKE_SOURCE_DIR}/lib/cxxopts/include/
	${CMAKE_SOURCE_DIR}/src/
)

if(CMAKE_HOST_UNIX)
    if(CONFIG STREQUAL "Debug")
        add_compile_options(-Wall -O0 -ggdb)
    else()
        add_compile_options(-fopenmp -O3)
        add_definitions(-DNDEBUG)
    endif()
endif()

## lib
add_library(Eigen ${CMAKE_SOURCE_DIR}/lib/Eigen/)
set_target_properties(Eigen PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(Eigen PUBLIC
	${CMAKE_SOURCE_DIR}/lib/Eigen/
)

add_library(snap ${CMAKE_SOURCE_DIR}/lib/snap-core/Snap.cpp)
target_include_directories(snap PUBLIC
	${CMAKE_SOURCE_DIR}/lib/glib-core/
	${CMAKE_SOURCE_DIR}/lib/snap-adv/
	${CMAKE_SOURCE_DIR}/lib/snap-core/
)

add_library(force_directed
    ${CMAKE_SOURCE_DIR}/src/force_directed.h
	${CMAKE_SOURCE_DIR}/src/layout.cpp
	${CMAKE_SOURCE_DIR}/src/graphs.cpp
)
target_link_libraries(force_directed snap)

## executables
add_executable(example src/demo-TNEANet.cpp)
target_link_libraries(example snap force_directed)

add_executable(animate_spring src/animate_spring.cpp)
target_link_libraries(animate_spring snap force_directed)

add_executable(animate_tutte src/animate_barycenter.cpp)
target_link_libraries(animate_tutte snap force_directed)

add_executable(tutte_prism src/barycenter_prism.cpp)
target_link_libraries(tutte_prism snap force_directed)

## tree executables
add_executable(tree_draw src/tree.cpp src/tree_draw.cpp src/tree_main.cpp)
add_executable(tree_paper src/tree.cpp src/tree_paper.cpp src/tree_draw.cpp)