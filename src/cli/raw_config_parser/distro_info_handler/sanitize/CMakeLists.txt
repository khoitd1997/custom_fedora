include(GoogleTest)

add_library(raw_config_parser_distro_info_sanitize distro_info_sanitize.cpp)
target_link_libraries(raw_config_parser_distro_info_sanitize
                      PUBLIC toml11 raw_config_parser_common_error
                             raw_config_parser_common_type
                      PRIVATE raw_config_parser_common_sanitize formatter
                              raw_config_parser_utils)
target_include_directories(raw_config_parser_distro_info_sanitize
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(raw_config_parser_distro_info_sanitize_test
               ${CMAKE_CURRENT_SOURCE_DIR}/test/distro_info_sanitize_test.cpp)
target_link_libraries(raw_config_parser_distro_info_sanitize_test
                      PRIVATE raw_config_parser_distro_info_sanitize
                              raw_config_parser_common_sanitize
                              raw_config_parser_test_utils
                              utils
                              gmock_custom_main)
gtest_discover_tests(raw_config_parser_distro_info_sanitize_test
                     WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/test)
