cmake_minimum_required(VERSION 3.16.3)
project(MQTTC_UET)

set(CMAKE_CXX_STANDARD 11)

file(GLOB SERVER_SRC "server/*.c")

add_executable(server ${SERVER_SRC})
target_link_libraries(server MQTTC_UET)