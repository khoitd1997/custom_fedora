project(Raw_config_parser_package)

add_library(raw_config_parser_package repo_parser.cpp)
target_link_libraries(
  raw_config_parser_package
  PUBLIC toml11 raw_config_parser_common_error raw_config_parser_common_type
  PRIVATE utils raw_config_parser_common_sanitize toml_utils)

target_include_directories(raw_config_parser_package
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
