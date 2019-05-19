add_library(magic_enum INTERFACE)

target_compile_options(magic_enum INTERFACE -Wall)
target_include_directories(magic_enum SYSTEM
                           INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
