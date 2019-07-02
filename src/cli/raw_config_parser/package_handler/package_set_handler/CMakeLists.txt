add_subdirectory(sanitize)

file(GLOB CppSources *.cpp)
add_library(raw_config_parser_package_set ${CppSources})
target_link_libraries(raw_config_parser_package_set
                      PUBLIC toml11 raw_config_parser_common_error
                             hatter_config_type
                      PRIVATE utils
                              magic_enum
                              raw_config_parser_common_sanitize
                              raw_config_parser_utils
                              raw_config_parser_package_set_sanitize
                              toml_utils)

target_include_directories(raw_config_parser_package_set
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
