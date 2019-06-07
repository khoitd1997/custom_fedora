include(GoogleTest)

add_subdirectory(sanitize)
add_subdirectory(get)

add_library(raw_config_parser_distro_info distro_info_handler.cpp)
target_link_libraries(raw_config_parser_distro_info
                      PUBLIC toml11 raw_config_parser_common_error
                             raw_config_parser_common_type
                      PRIVATE raw_config_parser_distro_info_sanitize
                              raw_config_parser_distro_info_get
                              raw_config_parser_utils
                              utils
                              toml_utils)
target_include_directories(raw_config_parser_distro_info
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
