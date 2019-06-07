file(GLOB CppSources *.cpp)
add_library(raw_config_parser_common_type ${CppSources})
target_link_libraries(raw_config_parser_common_type PUBLIC magic_enum)
target_include_directories(raw_config_parser_common_type
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
