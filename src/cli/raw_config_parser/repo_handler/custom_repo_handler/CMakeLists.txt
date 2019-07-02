file(GLOB CppSources *.cpp)
add_library(raw_config_parser_custom_repo ${CppSources})
target_link_libraries(
  raw_config_parser_custom_repo
  PUBLIC toml11 raw_config_parser_common_error hatter_config_type
  PRIVATE utils raw_config_parser_common_sanitize toml_utils)

target_include_directories(raw_config_parser_custom_repo
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
