file(GLOB CppSources *.cpp)
add_library(config_decoder ${CppSources})
target_link_libraries(config_decoder PUBLIC hatter_config_type PRIVATE utils)
target_include_directories(config_decoder
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
