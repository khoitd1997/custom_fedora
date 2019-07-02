file(GLOB CppSources *.cpp)
add_library(build_variable ${CppSources})
# target_link_libraries(build_variable)
target_include_directories(build_variable
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
