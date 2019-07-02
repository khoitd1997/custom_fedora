add_subdirectory(sanitize)
add_subdirectory(package_set_handler)

file(GLOB CppSources *.cpp)
add_library(raw_config_parser_package ${CppSources})
target_link_libraries(raw_config_parser_package
                      PUBLIC toml11 raw_config_parser_common_error
                             hatter_config_type
                      PRIVATE utils
                              formatter
                              magic_enum
                              raw_config_parser_package_set
                              raw_config_parser_common_sanitize
                              raw_config_parser_package_sanitize
                              toml_utils)

target_include_directories(raw_config_parser_package
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
