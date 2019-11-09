cmake_minimum_required(VERSION 2.8)

project(Proteus_Client_Server_Test)
set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/../)
include_directories(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
add_definitions(-Wall -O0)
add_executable("Prot_Server" "SocketServer.cpp")
add_executable("Prot_Client" "SocketClient.cpp")
add_executable("Test_Parse" "test/test_parse.cpp")
add_executable("Test_UDP" "test/test_udp.cpp")
add_executable("Test_TCP" "test/test_tcp.cpp")



