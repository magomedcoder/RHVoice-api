cmake_minimum_required(VERSION 3.22)
project(RHvoice-api)

set(CMAKE_CXX_STANDARD 23)

find_package(cpprestsdk REQUIRED)

add_executable(rhvoice_tts_server src/main.cpp)

target_link_libraries(rhvoice_tts_server cpprestsdk::cpprest)