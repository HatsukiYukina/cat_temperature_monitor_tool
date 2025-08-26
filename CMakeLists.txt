cmake_minimum_required(VERSION 3.25.1)
project(cat_temperature_monitor_tool C)

set(CMAKE_C_STANDARD 11)

add_library(cJSON STATIC cJSON.c)
add_executable(cat_temperature_monitor_tool main.c)
