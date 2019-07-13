file(GLOB CppSources *.cpp)
add_library(logger ${CppSources})
target_include_directories(logger PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(logger PRIVATE build_variable)
