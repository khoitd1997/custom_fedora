project(Toml_utils)

add_library(toml_utils toml_utils.cpp)
target_link_libraries(toml_utils
                      PUBLIC toml11 raw_config_parser_common_error
                             raw_config_parser_common_type)

target_include_directories(toml_utils
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
