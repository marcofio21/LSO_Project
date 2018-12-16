cmake_minimum_required(VERSION 3.12)
project(_LSO_Project C)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11 -pthread")

set(CMAKE_C_STANDARD 11)

add_executable(_LSO_Project server_main.c substratum_server.c substratum_server.h Lists/list_library.c Lists/list_library.h message_func_server.c message_func_server.h Lists/list_addr/list_addr_functions.c Lists/list_addr/list_addr_functions.h)

