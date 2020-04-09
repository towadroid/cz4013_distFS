cmake_minimum_required(VERSION 3.11)
project(server)

# use C++17 to use shared_ptr with arrays
set(CMAKE_CXX_STANDARD 17)

# FetchContent_MakeAvailable is only available as a command since 3.14
# However FetchContent is available since 3.11
# see https://cliutils.gitlab.io/modern-cmake/chapters/projects/fetch.html
if (${CMAKE_VERSION} VERSION_LESS 3.14)
    macro(FetchContent_MakeAvailable NAME)
        FetchContent_GetProperties(${NAME})
        if (NOT ${NAME}_POPULATED)
            FetchContent_Populate(${NAME})
            add_subdirectory(${${NAME}_SOURCE_DIR} ${${NAME}_BINARY_DIR})
        endif ()
    endmacro()
endif ()
# end workaround

######
# download libraries from git repo and make their targets available to this build
# https://cmake.org/cmake/help/latest/module/FetchContent.html
include(FetchContent)
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG master
        UPDATE_DISCONNECTED ON
        #online download once, afterwards no updates for consecutive builds
        #otherwise cannot build offline
)
FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG v1.5.0
        UPDATE_DISCONNECTED ON
)

FetchContent_MakeAvailable(googletest)
FetchContent_MakeAvailable(spdlog)
######

add_executable(server main.cpp constants.hpp UdpServer_linux.cpp UdpServer_linux.hpp utils/utils.cpp utils/utils.hpp Handler.cpp Handler.hpp HelperClasses/MonitoringClient.cpp HelperClasses/MonitoringClient.hpp HelperClasses/HelperClasses.hpp utils/packing.cpp utils/packing.hpp utils/packing_template_impl.hpp HelperClasses/ReceiveSpecifier.cpp HelperClasses/ReceiveSpecifier.hpp)

add_executable(develop main_dev.cpp constants.hpp UdpServer_linux.cpp UdpServer_linux.hpp utils/utils.cpp utils/utils.hpp Handler.cpp Handler.hpp HelperClasses/MonitoringClient.cpp HelperClasses/MonitoringClient.hpp HelperClasses/HelperClasses.hpp utils/packing.cpp utils/packing.hpp utils/packing_template_impl.hpp HelperClasses/ReceiveSpecifier.cpp HelperClasses/ReceiveSpecifier.hpp)

add_executable(tests constants.hpp UdpServer_linux.cpp UdpServer_linux.hpp utils/utils.cpp utils/utils.hpp Handler.cpp Handler.hpp HelperClasses/MonitoringClient.cpp HelperClasses/MonitoringClient.hpp server_tests/basic_tests/utils_test.cpp HelperClasses/HelperClasses.hpp utils/packing.cpp utils/packing.hpp server_tests/handler_tests/handler_test.cpp server_tests/mock_classes/MockHandler.hpp utils/packing_template_impl.hpp server_tests/basic_tests/cache_bounds.cpp server_tests/mock_classes/MockUdpServer_linux.hpp server_tests/basic_tests/file_ops.cpp server_tests/handler_tests/handler_overall_tests.cpp server_tests/test_resoures.cpp server_tests/test_resoures.hpp server_tests/handler_tests/monitoring_test.cpp HelperClasses/ReceiveSpecifier.cpp HelperClasses/ReceiveSpecifier.hpp server_tests/handler_tests/atmost_test.cpp server_tests/handler_tests/file_does_not_exist.cpp server_tests/handler_tests/failing_reqeusts.cpp server_tests/handler_tests/fragment_test.cpp server_tests/handler_tests/send_failure_test.cpp)

#spdlog_enable_warnings(server)
target_link_libraries(server PRIVATE spdlog)

#spdlog_enable_warnings(develop)
target_link_libraries(develop PRIVATE spdlog)

#spdlog_enable_warnings(tests)
target_link_libraries(tests PRIVATE spdlog gtest_main gmock_main)
