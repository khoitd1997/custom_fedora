add_subdirectory(sanitize)

file(GLOB CppSources *.cpp)
add_library(raw_config_parser_build_process ${CppSources})
target_link_libraries(raw_config_parser_build_process
                      PUBLIC toml11 raw_config_parser_common_error
                             hatter_config_type
                      PRIVATE raw_config_parser_build_process_sanitize
                              raw_config_parser_utils
                              utils
                              toml_utils)

target_include_directories(raw_config_parser_build_process
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
