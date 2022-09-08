cmake_minimum_required(VERSION 3.10)

# set the project name
project(Pedalboard VERSION 1.0)

add_library(Pedalboard_lib_logger logger.cpp logger.hpp)

add_library(Pedalboard_lib_arduino arduino.cpp arduino.hpp)
target_include_directories(Pedalboard_lib_arduino PUBLIC include)

add_library(Pedalboard_lib_zoom zoomg3xn.cpp zoomg3xn.hpp)
target_include_directories(Pedalboard_lib_zoom PUBLIC include)

add_executable(Pedalboard_main middleware.cpp)

target_link_libraries(Pedalboard_main PRIVATE Pedalboard_lib_logger)
target_link_libraries(Pedalboard_main PRIVATE Pedalboard_lib_arduino)
target_link_libraries(Pedalboard_main PRIVATE Pedalboard_lib_zoom)
