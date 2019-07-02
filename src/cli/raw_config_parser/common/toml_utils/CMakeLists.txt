file(GLOB CppSources *.cpp)
add_library(toml_utils ${CppSources})
target_link_libraries(toml_utils
                      PUBLIC toml11
                             raw_config_parser_common_error
                             hatter_config_type
                             utils
                      PRIVATE formatter)

target_include_directories(toml_utils
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
