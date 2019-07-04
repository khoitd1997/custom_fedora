cmake_minimum_required(VERSION 3.13)
project(Hattercli LANGUAGES CXX)
include(FetchContent)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

FetchContent_Declare(
  util_cmake_content
  GIT_REPOSITORY https://github.com/khoitd1997/util_cmake_modules.git
  GIT_TAG origin/master)
FetchContent_MakeAvailable(util_cmake_content)
add_subdirectory(thirdparty)

util_check_and_enable_test() # -DRUN_TEST=ON
util_add_cpplint() # -DUSE_CPPLINT=ON
util_add_cppcheck() # -DUSE_CPPCHECK=ON

# TODO(kd): resolve clang bug in magic_enum and research check for clang-tidy
if(USE_CLANG-TIDY)
  find_program(CMAKE_CXX_CLANG_TIDY NAMES clang-tidy)
  list(
    APPEND
      CMAKE_CXX_CLANG_TIDY
      "-checks=-*,clang-analyzer-*,-clang-analyzer-cplusplus*,bugprone-*,cppcoreguidelines-*,google-*,misc-*,modernize-*,performance-*,portability-*,readability-*"
    )
endif()

util_set_general_code_gen_option()

add_subdirectory(utils)
add_subdirectory(test_utils)
add_subdirectory(logger)
add_subdirectory(formatter)

add_subdirectory(build_variable)
add_subdirectory(hatter_config_type)
add_subdirectory(config_builder)
add_subdirectory(raw_config_parser)
add_subdirectory(config_decoder)

add_executable(tomlparser ${PROJECT_SOURCES} toml_parser.cpp)
target_link_libraries(tomlparser
                      PRIVATE toml11
                              utils
                              build_variable
                              raw_config_parser
                              config_builder
                              config_decoder
                              logger)
