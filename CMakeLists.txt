cmake_minimum_required(VERSION 3.16.3)
project(MQTTC_UET VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 11)

#COMPILER
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -pthread")

#BUILD LIB
add_subdirectory(library)

#SOURCE
file(GLOB SERVER_SRC "src/server/*.c")
file(GLOB CLIENT_SRC "src/client/*.c")
file(GLOB AUTOCLIENT_SRC "src/auto/*.c")

add_executable(client ${CLIENT_SRC})
target_link_libraries(client library)

add_executable(server ${SERVER_SRC})
target_link_libraries(server library)

add_executable(autoclient ${AUTOCLIENT_SRC})
target_link_libraries(autoclient library)