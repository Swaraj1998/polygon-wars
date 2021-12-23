cmake_minimum_required(VERSION 3.2)
project(polygon-wars)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

aux_source_directory(src PROJECT_SRCS)
include_directories(src)

add_executable(main main.cpp ${PROJECT_SRCS})

if (${CMAKE_CXX_COMPILER_ID} MATCHES "Clang" OR
        ${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic -Wall -Wextra -Weffc++")
endif ()

find_package(SFML 2.5 COMPONENTS system window graphics network audio REQUIRED)
target_link_libraries(main
    sfml-system sfml-window sfml-graphics sfml-network sfml-audio
)