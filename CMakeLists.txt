cmake_minimum_required(VERSION 3.24)
project(SOProyecto LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)

# OpenSSL (SHA-256)
find_package(OpenSSL REQUIRED)

add_executable(flujo src/main.cpp)
target_link_libraries(flujo PRIVATE OpenSSL::SSL OpenSSL::Crypto)
