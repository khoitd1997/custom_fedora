project(Raw_config_parser)

add_library(raw_config_parser raw_config_parser.cpp toml_utils.cpp)
target_link_libraries(raw_config_parser PUBLIC toml11 PRIVATE utils)
target_include_directories(raw_config_parser
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
                           PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
