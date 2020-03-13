cmake_minimum_required(VERSION 3.1)
project(server)

set(CMAKE_CXX_STANDARD 14)

include(./lib/spdlog/cmake/utils.cmake)

if(NOT TARGET spdlog)
    # Stand-alone build
    find_package(spdlog REQUIRED)
endif()

#########
execute_process(COMMAND echo "${CMAKE_COMMAND}" -G "${CMAKE_GENERATOR} ${CMAKE_BINARY_DIR}")
# Download and unpack googletest at configure time
configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
execute_process(COMMAND "${CMAKE_COMMAND}" -G "${CMAKE_GENERATOR}" .
    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/googletest-download"
)
execute_process(COMMAND "${CMAKE_COMMAND}" --build .
    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/googletest-download"
)

# Prevent GoogleTest from overriding our compiler/linker options
# when building with Visual Studio
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This adds the following targets:
# gtest, gtest_main, gmock and gmock_main
add_subdirectory("${CMAKE_BINARY_DIR}/googletest-src"
                 "${CMAKE_BINARY_DIR}/googletest-build"
)

# The gtest/gmock targets carry header search path dependencies
# automatically when using CMake 2.8.11 or later. Otherwise we
# have to add them here ourselves.
if(CMAKE_VERSION VERSION_LESS 2.8.11)
    include_directories("${gtest_SOURCE_DIR}/include"
                        "${gmock_SOURCE_DIR}/include"
    )
endif()
##############


add_executable(server main.cpp constants.hpp globalvar.hpp UdpServer_linux.cpp UdpServer_linux.hpp utils.cpp utils.hpp Handler.cpp Handler.hpp MonitoringClient.cpp MonitoringClient.hpp)

add_executable(develop main_dev.cpp constants.hpp globalvar.hpp UdpServer_linux.cpp UdpServer_linux.hpp utils.cpp utils.hpp Handler.cpp Handler.hpp MonitoringClient.cpp MonitoringClient.hpp server_tests/basic_tests/unit_test1.cpp)

add_executable(newtest main_dev.cpp constants.hpp globalvar.hpp UdpServer_linux.cpp UdpServer_linux.hpp utils.cpp utils.hpp Handler.cpp Handler.hpp MonitoringClient.cpp MonitoringClient.hpp server_tests/basic_tests/unit_test1.cpp)

spdlog_enable_warnings(develop)
target_link_libraries(develop PRIVATE spdlog::spdlog)
target_link_libraries(newtest PRIVATE spdlog::spdlog gtest_main)
