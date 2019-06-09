file(GLOB CppSources *.cpp)
add_library(build_variable_manager ${CppSources})
# target_link_libraries(build_variable_manager)
target_include_directories(build_variable_manager
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
