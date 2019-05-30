file(GLOB CppSources *.cpp)
add_library(raw_config_parser_common_get ${CppSources})
target_link_libraries(raw_config_parser_common_get
                      PUBLIC toml11
                             raw_config_parser_common_error
                             toml_utils
                             raw_config_parser_utils
                      PRIVATE utils formatter)

target_include_directories(raw_config_parser_common_get
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

include(GoogleTest)
file(GLOB CppTestSources test/*.cpp)
add_executable(raw_config_parser_common_get_test ${CppTestSources})
target_link_libraries(raw_config_parser_common_get_test
                      PRIVATE raw_config_parser_common_get gmock_custom_main)
gtest_discover_tests(raw_config_parser_common_get_test WORKING_DIRECTORY
                     ${CMAKE_CURRENT_SOURCE_DIR}/test)
