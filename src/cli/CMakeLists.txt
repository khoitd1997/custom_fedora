cmake_minimum_required (VERSION 3.1)
project (Hattercli LANGUAGES CXX)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set (CMAKE_CXX_STANDARD 14)

add_subdirectory(thirdparty/cpptoml)

add_executable(tomlparser toml_parser.cpp ${PROJECT_SOURCES})
target_compile_options(tomlparser PRIVATE -Werror -Wall -Wextra)
target_link_libraries(tomlparser PRIVATE cpptoml)