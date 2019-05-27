add_subdirectory(sanitize)

file(GLOB CppSources *.cpp)
add_library(raw_config_parser_misc ${CppSources})
target_link_libraries(raw_config_parser_misc
                      PUBLIC toml11 raw_config_parser_common_error
                             raw_config_parser_common_type
                      PRIVATE raw_config_parser_misc_sanitize
                              raw_config_parser_utils toml_utils)

target_include_directories(raw_config_parser_misc
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
