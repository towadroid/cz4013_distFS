# cmake_minimum_required(VERSION <specify CMake version here>)
project(server)

set(CMAKE_CXX_STANDARD 14)

add_executable(server main.cpp constants.hpp globalvar.hpp UdpServer_linux.cpp UdpServer_linux.hpp utils.cpp utils.hpp Handler.cpp Handler.hpp MonitoringClient.cpp MonitoringClient.hpp)

add_executable(develop main_dev.cpp constants.hpp globalvar.hpp UdpServer_linux.cpp UdpServer_linux.hpp utils.cpp utils.hpp Handler.cpp Handler.hpp MonitoringClient.cpp MonitoringClient.hpp)
