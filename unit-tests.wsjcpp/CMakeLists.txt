# Automaticly generated by wsjcpp@v0.2.2
cmake_minimum_required(VERSION 3.0)

project(unit-tests C CXX)
add_definitions(-DWSJCPP_APP_VERSION="ut-v0.0.1")
add_definitions(-DWSJCPP_APP_NAME="unit-tests-wsjcpp-parser-cpp")

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(MACOSX TRUE)
endif()

set(CMAKE_CXX_STANDARD 17)
set(EXECUTABLE_OUTPUT_PATH ${unit-tests_SOURCE_DIR})

set (WSJCPP_LIBRARIES "")
set (WSJCPP_INCLUDE_DIRS "")
set (WSJCPP_SOURCES "")

find_package(Threads REQUIRED)
list (APPEND WSJCPP_LIBRARIES ${CMAKE_THREAD_LIBS_INIT})

# wsjcpp-core:v0.2.3
list (APPEND WSJCPP_INCLUDE_DIRS "../src.wsjcpp/wsjcpp_core/")
list (APPEND WSJCPP_SOURCES "../src.wsjcpp/wsjcpp_core/wsjcpp_core.cpp")
list (APPEND WSJCPP_SOURCES "../src.wsjcpp/wsjcpp_core/wsjcpp_core.h")
list (APPEND WSJCPP_SOURCES "../src.wsjcpp/wsjcpp_core/wsjcpp_unit_tests.cpp")
list (APPEND WSJCPP_SOURCES "../src.wsjcpp/wsjcpp_core/wsjcpp_unit_tests.h")
list (APPEND WSJCPP_SOURCES "../src.wsjcpp/wsjcpp_core/wsjcpp_unit_tests_main.cpp")

# wsjcpp-parser-cpp:v0.0.1
list (APPEND WSJCPP_INCLUDE_DIRS "../src")
list (APPEND WSJCPP_SOURCES "../src/wsjcpp_parser_cpp.cpp")
list (APPEND WSJCPP_SOURCES "../src/wsjcpp_parser_cpp.h")

# unit-tests
list (APPEND WSJCPP_INCLUDE_DIRS "src")
list (APPEND WSJCPP_SOURCES "../unit-tests.wsjcpp/src/unit_test_layer1.cpp")
list (APPEND WSJCPP_SOURCES "../unit-tests.wsjcpp/src/unit_test_layer0.cpp")

include(${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.user-custom.txt)


include_directories(${WSJCPP_INCLUDE_DIRS})

add_executable ("unit-tests" ${WSJCPP_SOURCES})

target_link_libraries("unit-tests" -lpthread ${WSJCPP_LIBRARIES} )

