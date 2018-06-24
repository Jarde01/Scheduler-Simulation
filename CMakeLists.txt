cmake_minimum_required(VERSION 3.5)
project(a3 C)

set(CMAKE_C_STANDARD 99)


#add_library(message message.c message.h)
#add_library(Server server.c server.h)
#add_library(Client client.c client.h)
#add_executable(server server.c)
#add_executable(client client.c)
#target_link_libraries(server pthread message)

add_executable(a3 simulation.c simulation.h)