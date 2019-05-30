Must have init run and then install can run

in var/lib/mock is the chroot, clean will remove it

/var/lib/name, name is determined by root var, different chroot won't share same dnf cache

Mock refreshes dnf.conf every invocation

Maybe /libexec for all other files and just the main one in /usr/bin

Support classic kickstart file but with additional settings through the TOML file like caching.

## Building from Source

### Dependency

The project uses C++17 features so to compile it, the newer gcc/clang are needed. The project uses gtest for unit testing as well as cppcheck and cpplint for extra linting, the default cmake won't turn these options on.

```shell
# library
sudo dnf install spdlog libasan

# for bare minimum build tool
sudo dnf install ninja-build cmake gcc-c++ clang

# for extra stuffs
sudo dnf install cppcheck gtest-devel gtest
pip3 install cpplint
```

### Build

```shell
# assume in source directory
mkdir -p build
cd build
cmake -G Ninja .. && ninja # bare minimum build
cmake -G Ninja -DUSE_CPPLINT=ON -DUSE_CPPCHECK=ON .. && ninja  # full build with unit test and linting

cd bin
```

## Build Stages

- First stage: Outside of mock
  - Handle TOML wrong syntax
  - Sanitize command line options
  - Sanitize all sections possible
- Second stage: Inside mock
  - Sanitize packages, repo
  - Control build scripts, enable caching, etc

## Format of generated env file from parser

The file will be called ```toml_parsed_env``` and will contain env variables used for building the final image

Sample:

```shell
# inside toml_parsed_env

env_os_name="fedora-kd"
env_enable_custom_cache=true
```

- Commandline argument:
  - Rebuild flag
  - Clear cache flag
  - Parse/Sanitize only
  - Build output dir
  - Config file/dir
  - Fedora version(default current system)
  - Fedora arch(default current arch)
