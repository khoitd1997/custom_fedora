include(GoogleTest)

add_library(gmock_custom_main gmock_custom_main.cpp)
target_link_libraries(gmock_custom_main PUBLIC gmock)
target_include_directories(gmock_custom_main
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
