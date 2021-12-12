cmake_minimum_required(VERSION 3.16.3)

# COMPILER
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -pthread")

set(LIBRARY_DIR ${PROJECT_SOURCE_DIR}/library)

# SOURCE
file(GLOB LIBRARY_SRC ${PROJECT_SOURCE_DIR}/library/*.c)

# BUILD LIB
add_library(library STATIC ${LIBRARY_SRC})
target_include_directories(library PUBLIC ${LIBRARY_DIR})

