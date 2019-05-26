file(GLOB CppSources *.cpp)
add_library(raw_config_parser_common_error ${CppSources})
target_link_libraries(raw_config_parser_common_error
                      PUBLIC utils formatter
                      PRIVATE logger)
target_include_directories(raw_config_parser_common_error
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
