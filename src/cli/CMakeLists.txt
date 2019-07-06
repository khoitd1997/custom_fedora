cmake_minimum_required(VERSION 3.13)
project(Hattercli LANGUAGES CXX)
include(FetchContent)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

FetchContent_Declare(
  util_cmake_content
  GIT_REPOSITORY https://github.com/khoitd1997/util_cmake_modules.git
  GIT_TAG origin/master)
FetchContent_MakeAvailable(util_cmake_content)
util_misc_init()

add_subdirectory(thirdparty)

util_check_and_enable_test() # -DRUN_TEST=ON

util_add_cpplint() # -DUSE_CPPLINT=ON

util_add_cppcheck() # -DUSE_CPPCHECK=ON

# util_add_clang_tidy() # -DUSE_CLANG-TIDY=ON
util_set_general_code_gen_option()
set(CXX_STANDARD_REQUIRED ON)

add_subdirectory(utils)
add_subdirectory(test_utils)
add_subdirectory(logger)
add_subdirectory(formatter)

add_subdirectory(build_variable)
add_subdirectory(hatter_config_type)
add_subdirectory(config_builder)
add_subdirectory(raw_config_parser)
add_subdirectory(config_decoder)

add_executable(hatter_cli ${PROJECT_SOURCES} hatter_cli.cpp)
target_link_libraries(hatter_cli
                      PRIVATE toml11
                              utils
                              build_variable
                              raw_config_parser
                              config_builder
                              config_decoder
                              logger)
