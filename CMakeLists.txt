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

file(GLOB_RECURSE glpk_files "*.c")
add_library(glpk ${glpk_files})
set_target_properties(glpk PROPERTIES LINKER_LANGUAGE C)
target_include_directories(glpk PUBLIC
	${CMAKE_SOURCE_DIR}/lib/glpk/src/
	${CMAKE_SOURCE_DIR}/lib/glpk/src/amd
	${CMAKE_SOURCE_DIR}/lib/glpk/src/api
	${CMAKE_SOURCE_DIR}/lib/glpk/src/bflib
	${CMAKE_SOURCE_DIR}/lib/glpk/src/colamd
	${CMAKE_SOURCE_DIR}/lib/glpk/src/draft
	${CMAKE_SOURCE_DIR}/lib/glpk/src/env
	${CMAKE_SOURCE_DIR}/lib/glpk/src/intopt
	${CMAKE_SOURCE_DIR}/lib/glpk/src/minisat
	${CMAKE_SOURCE_DIR}/lib/glpk/src/misc
	${CMAKE_SOURCE_DIR}/lib/glpk/src/mpl
	${CMAKE_SOURCE_DIR}/lib/glpk/src/npp
	${CMAKE_SOURCE_DIR}/lib/glpk/src/proxy
	${CMAKE_SOURCE_DIR}/lib/glpk/src/simplex
	${CMAKE_SOURCE_DIR}/lib/glpk/src/zlib
)

add_library(force_directed
    ${CMAKE_SOURCE_DIR}/src/force_directed.h
	${CMAKE_SOURCE_DIR}/src/layout.cpp
	${CMAKE_SOURCE_DIR}/src/graphs.cpp
)
target_link_libraries(force_directed snap)

## executables
add_executable(eades84 src/eades84.cpp)
target_link_libraries(eades84 snap force_directed)

add_executable(animate_spring src/animate_spring.cpp)
target_link_libraries(animate_spring snap force_directed)

add_executable(animate_tutte src/animate_barycenter.cpp)
target_link_libraries(animate_tutte snap force_directed)

add_executable(tutte_prism src/barycenter_prism.cpp)
target_link_libraries(tutte_prism snap force_directed)

## tree executables
add_executable(tree_draw src/tree.cpp src/tree_draw.cpp src/tree_main.cpp)
add_executable(tree_paper src/tree.cpp src/tree_paper.cpp src/tree_draw.cpp)

## tree_lp
add_executable(tree_lp src/tree_lp.cpp)
target_link_libraries(tree_lp glpk)

# add_executable(tree_test src/tree_lp.cpp tests/tree_tests.cpp)
# target_link_libraries(tree_test glpk)