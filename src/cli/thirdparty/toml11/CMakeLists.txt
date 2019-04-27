project(Toml11)

# Header only library, therefore INTERFACE
add_library(toml11 INTERFACE)

target_compile_options(toml11 INTERFACE -Wall)
target_include_directories(toml11 SYSTEM INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
