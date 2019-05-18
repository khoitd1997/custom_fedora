
file(GLOB CppSources *.cpp)
add_library(config_builder ${CppSources})
target_link_libraries(config_builder PRIVATE utils PUBLIC raw_config_parser)
target_include_directories(config_builder
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
