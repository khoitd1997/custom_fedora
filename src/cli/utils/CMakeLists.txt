file(GLOB CppSources *.cpp)
add_library(utils ${CppSources})
target_include_directories(utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
