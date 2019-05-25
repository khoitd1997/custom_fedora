include(GoogleTest)
find_package(GTest REQUIRED)

add_library(gtest_custom_main gtest_custom_main.cpp)
target_link_libraries(gtest_custom_main PUBLIC GTest::GTest)
target_include_directories(gtest_custom_main
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
