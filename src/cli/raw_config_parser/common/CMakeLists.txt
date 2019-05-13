project(Raw_config_parser_common)

add_library(raw_config_parser_common error_report_type.cpp error_type.cpp)
target_link_libraries(raw_config_parser_common)
target_include_directories(raw_config_parser_common
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
