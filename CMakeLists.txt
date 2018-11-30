cmake_minimum_required(VERSION 3.12)
project(_LSO_Project C)

set(CMAKE_C_STANDARD 11)

add_executable(_LSO_Project main.c substratum_server.c substratum_server.h)