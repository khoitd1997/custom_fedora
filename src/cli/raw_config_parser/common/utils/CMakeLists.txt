file(GLOB CppSources *.cpp)
add_library(raw_config_parser_utils ${CppSources})
target_link_libraries(raw_config_parser_utils
                      PUBLIC utils raw_config_parser_common_error)

target_include_directories(raw_config_parser_utils
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
