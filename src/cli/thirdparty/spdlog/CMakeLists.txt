project(Spdlog)

# Header only library, therefore INTERFACE
add_library(spdlog INTERFACE)

# INTERFACE targets only have INTERFACE properties
target_include_directories(spdlog INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/..)
