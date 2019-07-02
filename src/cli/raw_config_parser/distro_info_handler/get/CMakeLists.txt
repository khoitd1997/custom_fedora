include(GoogleTest)

add_library(raw_config_parser_distro_info_get distro_info_get.cpp)
target_link_libraries(raw_config_parser_distro_info_get
                      PUBLIC toml11 raw_config_parser_common_error
                             hatter_config_type
                      PRIVATE raw_config_parser_common_get toml_utils)
target_include_directories(raw_config_parser_distro_info_get
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(raw_config_parser_distro_info_get_test
               ${CMAKE_CURRENT_SOURCE_DIR}/test/distro_info_get_test.cpp)
target_link_libraries(raw_config_parser_distro_info_get_test
                      PRIVATE raw_config_parser_distro_info_get
                              raw_config_parser_common_get
                              raw_config_parser_test_utils
                              utils
                              gmock_custom_main)
gtest_discover_tests(raw_config_parser_distro_info_get_test WORKING_DIRECTORY
                     ${CMAKE_CURRENT_SOURCE_DIR}/test)
