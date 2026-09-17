cmake_minimum_required(VERSION 3.15)
project(meu_crud_cpp)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Crow CONFIG REQUIRED)
find_package(libpqxx CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

add_executable(meu_crud_cpp src/main.cpp
        src/models/Usuario.hpp
        src/repositories/UsuarioRepository.hpp)

target_link_libraries(meu_crud_cpp PRIVATE
    Crow::Crow
    libpqxx::pqxx
    nlohmann_json::nlohmann_json
)
