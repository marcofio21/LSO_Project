cmake_minimum_required(VERSION 3.12)
project(_LSO_Project C)

set(CMAKE_C_STANDARD 11)

set(CMAKE_USE_PTHREADS_INIT true)
set(CMAKE_USE_PTHREADS_INIT ON)

find_package(Threads REQUIRED)

add_executable(_LSO_Project main.c substratum_server.c substratum_server.h list_server_addr.c list_server_addr.h)

