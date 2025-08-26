cmake_minimum_required(VERSION 3.25.1)
project(cat_temperature_monitor_tool C)

set(CMAKE_C_STANDARD 11)

add_library(cJSON STATIC cJSON.c)
add_library(configreader STATIC configreader.c)
add_library(catlog STATIC catlog.c)
add_library(temperature STATIC temperature.c)
add_library(csvlogger STATIC csvlogger.c)
add_executable(cat_temperature_monitor_tool main.c
        include/configreader.h
        configreader.c
        include/catlog.h
        catlog.c
        include/temperature.h
        temperature.c
        include/csvlogger.h
        csvlogger.c)

target_link_libraries(cat_temperature_monitor_tool
        cJSON
        configreader
        catlog
        temperature
        csvlogger
)


