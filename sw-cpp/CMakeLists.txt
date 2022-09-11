cmake_minimum_required(VERSION 3.10)
set (CMAKE_CXX_STANDARD 11)

# set the project name
project(Pedalboard VERSION 1.0)
add_subdirectory(include)

# Setup for serial library -----
if(APPLE)
    find_library(IOKIT_LIBRARY IOKit)
    find_library(FOUNDATION_LIBRARY Foundation)
endif()

## Sources
set(serial_SRCS
    serial_src/serial.cc
    include/serial/serial.h
    include/serial/v8stdint.h
)
if(APPLE)
    # If OSX
    list(APPEND serial_SRCS serial_src/impl/unix.cc)
    list(APPEND serial_SRCS serial_src/impl/list_ports/list_ports_osx.cc)
elseif(UNIX)
    # If unix
    list(APPEND serial_SRCS serial_src/impl/unix.cc)
    list(APPEND serial_SRCS serial_src/impl/list_ports/list_ports_linux.cc)
else()
    # If windows
    list(APPEND serial_SRCS serial_src/impl/win.cc)
    list(APPEND serial_SRCS serial_src/impl/list_ports/list_ports_win.cc)
endif()

## Add serial library
add_library(serial ${serial_SRCS})
if(APPLE)
    target_link_libraries(serial ${FOUNDATION_LIBRARY} ${IOKIT_LIBRARY})
elseif(UNIX)
    target_link_libraries(serial rt pthread)
else()
    target_link_libraries(serial setupapi)
endif()

target_include_directories(serial PRIVATE include)
## End serial library setup ------

add_library(Pedalboard_lib_logger logger.cpp logger.hpp)

add_library(Pedalboard_lib_arduino arduino.cpp arduino.hpp)
target_include_directories(Pedalboard_lib_arduino PUBLIC include)
target_link_libraries(Pedalboard_lib_arduino PRIVATE serial)

add_library(Pedalboard_lib_zoom zoomg3xn.cpp zoomg3xn.hpp)
target_include_directories(Pedalboard_lib_zoom PUBLIC include)
target_link_libraries(Pedalboard_lib_zoom PRIVATE RtMidiLib)

add_executable(Pedalboard_main middleware.cpp)

target_link_libraries(Pedalboard_main PRIVATE Pedalboard_lib_logger)
target_link_libraries(Pedalboard_main PRIVATE Pedalboard_lib_arduino)
target_link_libraries(Pedalboard_main PRIVATE Pedalboard_lib_zoom)
