cmake_minimum_required(VERSION 3.16.3)
project(MQTTC_UET VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 11)

#COMPILER
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread")

#BUILD LIB
#add_library(MQTTC_UET STATIC ${LIBS})

#SOURCE
file(GLOB SERVER_SRC "server/*.c")
file(GLOB CLIENT_SRC "client/*.c")

add_executable(server ${SERVER_SRC})
#target_link_libraries(server MQTTC_UET)

add_executable(client ${CLIENT_SRC})
#target_link_libraries(client MQTTC_UET)