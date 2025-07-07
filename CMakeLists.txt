cmake_minimum_required(VERSION 3.24)
project(SOProyecto LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 98)

add_executable(flujo
    src/main.cpp
    src/cifrado.cpp
    src/hash.cpp
    src/verificar.cpp
)
