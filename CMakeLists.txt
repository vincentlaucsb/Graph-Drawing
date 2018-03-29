cmake_minimum_required(VERSION 3.9)
project(Force_Directed_Drawing)

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
include_directories(lib/svg/src/)

add_executable(example src/demo-TNEANet.cpp)
target_link_libraries(example snap)

add_executable(draw src/main.cpp lib/svg/src/svg.hpp)
target_link_libraries(draw snap)