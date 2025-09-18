cmake_minimum_required(VERSION 3.14)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(userver-service CXX)

find_package(
    userver
    COMPONENTS core
    REQUIRED
)

add_executable(${PROJECT_NAME} "main.cpp")
target_link_libraries(${PROJECT_NAME} userver::core)

userver_testsuite_add_simple()
