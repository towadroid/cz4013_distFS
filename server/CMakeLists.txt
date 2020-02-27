# cmake_minimum_required(VERSION <specify CMake version here>)
project(server)

set(CMAKE_CXX_STANDARD 14)

add_executable(server main.cpp constants.hpp globalvar.hpp UdpServer_win.cpp UdpServer_win.hpp)

# winsock library https://stackoverflow.com/a/15124455 and https://stackoverflow.com/a/46463552
if(WIN32)
    target_link_libraries(server ws2_32)
endif()
