cmake_minimum_required(VERSION 3.22)
project(RHVoice-api)

set(CMAKE_CXX_STANDARD 23)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.cpp" "main.cpp")
add_executable(RHVoice-api ${SOURCES})

find_package(cpprestsdk REQUIRED)
find_package(OpenSSL REQUIRED)

target_link_libraries(RHVoice-api PRIVATE cpprest OpenSSL::SSL OpenSSL::Crypto)