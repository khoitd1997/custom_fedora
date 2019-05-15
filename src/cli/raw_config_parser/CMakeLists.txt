project(Raw_config_parser)

add_subdirectory(common)
add_subdirectory(repo_handler)

file(GLOB CppSources *.cpp)
add_library(raw_config_parser ${CppSources})
target_link_libraries(raw_config_parser
                      PUBLIC raw_config_parser_common_type
                      PRIVATE utils
                              toml_utils
                              raw_config_parser_common_error
                              raw_config_parser_common_sanitize
                              raw_config_parser_repo)

target_include_directories(raw_config_parser
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
                           PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
