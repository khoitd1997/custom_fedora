cmake_minimum_required(VERSION 3.10)
project(Hattercli LANGUAGES CXX)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# static analysis and compile options
find_program(CMAKE_CXX_CPPCHECK NAMES cppcheck)
find_program(CMAKE_CXX_CPPLINT NAMES cpplint)
list(
  APPEND
    CMAKE_CXX_CPPLINT
    "--filter=-legal/copyright, -whitespace/line_length, -whitespace/ending_newline, -build/c++11"
    "--quiet")
list(APPEND CMAKE_CXX_CPPCHECK
            "--enable=all"
            "-q"
            "--force"
            "--suppressions-list=${CMAKE_SOURCE_DIR}/cppcheck_suppression.txt")
# set(CMAKE_CXX_INCLUDE_WHAT_YOU_USE "iwyu")
set(CMAKE_CXX_STANDARD 14)
add_compile_options(-Wall
                    -Wextra
                    -Werror
                    -fasynchronous-unwind-tables
                    -fstack-protector
                    -fstack-protector-strong
                    -fPIC
                    -shared
                    -pipe
                    -fsanitize=address)
add_link_options(-fsanitize=address)

add_subdirectory(thirdparty/cpptoml)
add_subdirectory(thirdparty/spdlog)

add_library(mockconfig mock_config.cpp)
target_link_libraries(mockconfig PRIVATE cpptoml spdlog)
target_compile_options(mockconfig PRIVATE -g)
target_include_directories(mockconfig PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(tomlparser toml_parser.cpp ${PROJECT_SOURCES})
target_compile_options(tomlparser PRIVATE -Werror -Wall -Wextra)
target_link_libraries(tomlparser PRIVATE cpptoml PRIVATE mockconfig spdlog)
