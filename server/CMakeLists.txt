cmake_minimum_required(VERSION 3.14)
project(server)

# use C++17 to use shared_ptr with arrays
set(CMAKE_CXX_STANDARD 17)

######
# download libraries from git repo and make their targets available to this build
# https://cmake.org/cmake/help/latest/module/FetchContent.html
include(FetchContent)
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG master
)
FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.5.0
)

FetchContent_MakeAvailable(googletest spdlog)
######

add_executable(server main.cpp constants.hpp globalvar.hpp UdpServer_linux.cpp UdpServer_linux.hpp utils.cpp utils.hpp Handler.cpp Handler.hpp MonitoringClient.cpp MonitoringClient.hpp)

add_executable(develop main_dev.cpp constants.hpp globalvar.hpp UdpServer_linux.cpp UdpServer_linux.hpp utils.cpp utils.hpp Handler.cpp Handler.hpp MonitoringClient.cpp MonitoringClient.hpp)

add_executable(newtest constants.hpp globalvar.hpp UdpServer_linux.cpp UdpServer_linux.hpp utils.cpp utils.hpp Handler.cpp Handler.hpp MonitoringClient.cpp MonitoringClient.hpp server_tests/basic_tests/unit_test1.cpp server_tests/basic_tests/utils_test.cpp)

spdlog_enable_warnings(develop)
target_link_libraries(develop PRIVATE spdlog)

spdlog_enable_warnings(newtest)
target_link_libraries(newtest PRIVATE spdlog gtest_main)
