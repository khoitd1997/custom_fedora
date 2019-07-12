file(GLOB CppSources *.cpp)
add_library(hatter_config_type ${CppSources})
target_link_libraries(hatter_config_type PUBLIC magic_enum toml11 PRIVATE utils)
target_include_directories(hatter_config_type
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
