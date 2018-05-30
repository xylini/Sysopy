cmake_minimum_required(VERSION 3.9.6)
project(zestaw7 C)

set(CMAKE_C_STANDARD 11)

add_executable(barber barber.c common.h)
add_executable(client client.c common.h)
