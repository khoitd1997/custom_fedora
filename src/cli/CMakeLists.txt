cmake_minimum_required (VERSION 3.0.0)
project (Hattercli LANGUAGES CXX)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(thirdparty/cpptoml)

add_executable(tomlparser toml_parser.cpp ${PROJECT_SOURCES})
target_link_libraries(tomlparser PRIVATE cpptoml)
# target_compile_features(JSONUtils PRIVATE cxx_std_11)
