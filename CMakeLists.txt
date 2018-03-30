cmake_minimum_required(VERSION 3.9)
project(Force_Directed_Drawing)
set(SOURCES src/graphs.cpp)

if(CMAKE_HOST_UNIX)
    if(CONFIG STREQUAL "Debug")
        add_compile_options(-Wall -O0 -ggdb)
    else()
        add_compile_options(-fopenmp -O3)
        add_definitions(-DNDEBUG)
    endif()
endif()

## lib
add_library(snap lib/snap-core/Snap.cpp)
target_include_directories(snap PUBLIC 
	lib/glib-core/
	lib/snap-adv/
	lib/snap-core/
)

## executables
include_directories(${CMAKE_SOURCE_DIR}/lib/svg/src/)
include_directories(${CMAKE_SOURCE_DIR}/src/)

add_executable(example src/demo-TNEANet.cpp)
target_link_libraries(example snap)

add_executable(draw src/graphs.cpp src/main.cpp)
target_link_libraries(draw snap)