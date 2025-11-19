cmake_minimum_required(VERSION 3.15)
project(CozyCricket LANGUAGES CXX)

set(CMAKE_EXE_LINKER_FLAGS "-static")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(cozycricket main.cpp)

find_package(OpenSSL REQUIRED)
find_package(cpr CONFIG REQUIRED)

target_link_libraries(cozycricket PRIVATE
    OpenSSL::SSL
    OpenSSL::Crypto
    cpr::cpr
)

