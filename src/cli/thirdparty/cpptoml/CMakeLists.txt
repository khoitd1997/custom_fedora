project (Cpptoml)

# Header only library, therefore INTERFACE
add_library(cpptoml INTERFACE)

# INTERFACE targets only have INTERFACE properties
target_include_directories(cpptoml INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)