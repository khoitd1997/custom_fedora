file(GLOB CppSources *.cpp)
add_library(raw_config_parser_package_sanitize ${CppSources})
target_link_libraries(raw_config_parser_package_sanitize
                      PUBLIC toml11
                             raw_config_parser_common_error
                             hatter_config_type
                             raw_config_parser_package
                      PRIVATE raw_config_parser_common_sanitize
                              formatter
                              magic_enum
                              raw_config_parser_package
                              raw_config_parser_utils)

target_include_directories(raw_config_parser_package_sanitize
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

include(GoogleTest)
file(GLOB CppTestSources test/*.cpp)
add_executable(raw_config_parser_package_sanitize_test ${CppTestSources})
target_link_libraries(raw_config_parser_package_sanitize_test
                      PRIVATE raw_config_parser_package_sanitize
                              raw_config_parser_common_sanitize
                              raw_config_parser_test_utils
                              utils
                              gmock_custom_main)
gtest_discover_tests(raw_config_parser_package_sanitize_test WORKING_DIRECTORY
                     ${CMAKE_CURRENT_SOURCE_DIR}/test)
