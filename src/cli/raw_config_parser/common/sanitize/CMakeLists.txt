project(Raw_config_parser_common_sanitize)

file(GLOB CppSources *.cpp)
add_library(raw_config_parser_common_sanitize ${CppSources})
target_link_libraries(raw_config_parser_common_sanitize
                      PUBLIC toml11 raw_config_parser_common_error
                      PRIVATE utils)

target_include_directories(raw_config_parser_common_sanitize
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
