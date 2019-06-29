#!/bin/bash

currDir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

cd ${currDir}/build

while (( "$#" )); do
    case "$1" in
        --clean)
        rm -rf ./*
        shift 1
        ;;
    esac
done
# rm -rf ${currDir}/build
mkdir -p ${currDir}/build/bin/build/logs
mkdir -p ${currDir}/build/googletest-src
mkdir -p ${currDir}/build/googletest-download
mkdir -p ${currDir}/build/googletest-build

ln -sfv ~/hatter/example/example_settings.toml ~/hatter/src/cli/build/bin
ln -sfv ~/hatter/example/child_setting_1.toml ~/hatter/src/cli/build/bin
mkdir -p ~/hatter/src/cli/build/bin/child_path
ln -sfv ~/hatter/example/child_path/child_setting_2.toml ~/hatter/src/cli/build/bin/child_path

# cmake .. && make && ./bin/tomlparser ./bin/example_settings.toml
# scan-build cmake -G Ninja .. && scan-build ninja && ./bin/tomlparser ./bin/settings.toml

set -e
# export GTEST_FILTER="CommonSanitizeTest_*" # MUST SPECIFY HERE BEFORE THE TESTS ARE DISCOVERED
cmake -G Ninja -DUSE_CPPLINT=ON -DUSE_CPPCHECK=ON -DRUN_TEST=ON .. && ninja 

# GTEST_BREAK_ON_FAILURE=1 GTEST_COLOR=1 ctest --verbose --gtest_print_time=0 
cd bin
mkdir -p out
set -e
if [ -f "build/prev_env_var.sh" ]; then 
    source build/prev_env_var.sh
    export env_is_first_build=false
else
    export env_is_first_build=true
fi
cat > ./build/env_var.sh << EOF
export env_parent_config=fedora_kd.toml
export env_os_name=fedora_kd

export env_releasever=29
export env_arch=x86_64

export env_clear_cache=true
export env_parser_mode=false
EOF
source ./build/env_var.sh
./tomlparser example_settings.toml
if [ $? -ne 0 ]; then
    echo "toml parser failed"
else
    echo "toml parser succeeded"
    cp build/env_var.sh build/prev_env_var.sh
    sed '/env_/s/env_/prev_env_/' -i build/prev_env_var.sh

    cp example_settings.toml build/prev_config.toml
fi
# cmake -G Ninja .. && ninja && ./bin/tomlparser ./bin/random.toml