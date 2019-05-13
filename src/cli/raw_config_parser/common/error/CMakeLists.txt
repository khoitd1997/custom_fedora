project(Raw_config_parser_common_error)

add_library(raw_config_parser_common_error error_report_type.cpp error_type.cpp)
target_link_libraries(raw_config_parser_common_error)
target_include_directories(raw_config_parser_common_error
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
