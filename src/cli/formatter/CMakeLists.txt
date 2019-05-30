file(GLOB CppSources *.cpp)
add_library(formatter ${CppSources})
target_include_directories(formatter PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
