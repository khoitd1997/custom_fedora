set(ENV{CC} /usr/bin/gcc)
set(ENV{CXX} /usr/bin/g++)
# set(ENV{CC} /usr/bin/clang)
# set(ENV{CXX} /usr/bin/clang++)
set (CMAKE_GENERATOR "Ninja" CACHE INTERNAL "" FORCE)
set (USE_CPPLINT ON CACHE INTERNAL "" FORCE)
set (USE_CPPCHECK ON CACHE INTERNAL "" FORCE)