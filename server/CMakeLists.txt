cmake_minimum_required(VERSION 3.1)
project(server)

set(CMAKE_CXX_STANDARD 14)

include(./lib/spdlog/cmake/utils.cmake)

if(NOT TARGET spdlog)
    # Stand-alone build
    find_package(spdlog REQUIRED)
endif()

add_executable(server main.cpp constants.hpp globalvar.hpp UdpServer_linux.cpp UdpServer_linux.hpp utils.cpp utils.hpp Handler.cpp Handler.hpp MonitoringClient.cpp MonitoringClient.hpp)

add_executable(develop main_dev.cpp constants.hpp globalvar.hpp UdpServer_linux.cpp UdpServer_linux.hpp utils.cpp utils.hpp Handler.cpp Handler.hpp MonitoringClient.cpp MonitoringClient.hpp)
spdlog_enable_warnings(develop)
target_link_libraries(develop PRIVATE spdlog::spdlog)