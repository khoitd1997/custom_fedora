util_add_external_header_lib(toml11
                             https://github.com/ToruNiina/toml11.git
                             origin/master
                             DEST_DIR
                             ${CMAKE_BINARY_DIR}/bin/toml11/toml11
                             INCLUDE_DIR
                             ${CMAKE_BINARY_DIR}/bin/toml11)
util_add_external_header_lib(magic_enum
                             https://github.com/Neargye/magic_enum.git
                             origin/master)

util_add_external_lib(googletest https://github.com/google/googletest.git
                      origin/master)
