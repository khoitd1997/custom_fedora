#!/bin/bash

set -eo pipefail
# set -x
hatter_src_dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
cmake_build_dir=/cmake_build_dir
if [ -z "${IN_HATTER_DOCKER}" ] && [ -z "${CI}" ]; then
    # container_engine=podman
    container_engine=docker
    
    cd ${hatter_src_dir}
    
    if [ ! -d "${hatter_src_dir}/cmake_build_dir" ]; then
        mkdir ${hatter_src_dir}/cmake_build_dir
        chmod 0777 -R ${hatter_src_dir}/cmake_build_dir
    fi
    
    # ${container_engine} rm -f test_instance
    ${container_engine} build -t hattertest --cpuset-cpus="0-3" .
    # ${container_engine} volume create cmake_build_vol
    # SYS_PTRACE because of sanitizer
    echo ${hatter_src_dir}/cmake_build_dir
    ${container_engine} run --tty --name test_instance --cap-add SYS_PTRACE --rm -v ${hatter_src_dir}/cmake_build_dir:${cmake_build_dir}:Z hattertest
else
    # rm -rf ${cmake_build_dir}/*
    if [ ! -z "${CI}" ]; then
        mkdir -p ${cmake_build_dir}
    fi
    # TODO(kd): Move this to end-to-end test place
    test_build_root_dir=${cmake_build_dir}/bin
    mkdir -p ${test_build_root_dir}
    rm -rf ${test_build_root_dir}/*
    mkdir -p ${test_build_root_dir}/build
    
    source ${hatter_src_dir}/scripts/build_utils.sh
    build_env_var_file "example_setting" "29" \
    "x86_64" "false" \
    "false" ${test_build_root_dir}/build/env_var.sh
    
    cd ${test_build_root_dir} # set pwd to test_build_root_dir
    source ${hatter_src_dir}/scripts/set_env_var.sh
    
    build_project_file_structure
    cp -r ${hatter_src_dir}/scripts/assets ${env_share_dir}
    cp -r ${hatter_src_dir}/example/* ${env_user_supplied_dir}
    
    # export GTEST_FILTER="CommonSanitizeTest_*" # MUST SPECIFY HERE BEFORE THE TESTS ARE DISCOVERED
    # scan-build cmake -G Ninja .. && scan-build ninja && ./bin/hatter_config_builder_parser ./bin/settings.toml
    cd ${cmake_build_dir}
    cmake ${hatter_src_dir} -DRUN_TEST=ON -DCMAKE_BUILD_TYPE=Debug
    cmake --build .
    
    # run unit tests
    # TODO(kd): Resolve gtest issue
    # cd bin
    # GTEST_BREAK_ON_FAILURE=1 GTEST_COLOR=1 ctest --verbose --gtest_print_time=0
    
    if [ -f "${env_prev_var_path}" ]; then
        source ${env_prev_var_path}
    fi
    ${test_build_root_dir}/hatter_config_builder_parser
    if [ $? -ne 0 ]; then
        echo "toml parser failed"
    else
        echo "toml parser succeeded"
        cp ${env_var_path} ${env_prev_var_path}
        sed '/env_/s/env_/prev_env_/' -i ${env_prev_var_path}
    fi
    
    # gitlab ci has bug with artifacts being in absolute path
    if [ ! -z "${CI}" ]; then
        cp ${test_build_root_dir}/hatter_config_builder_parser ${hatter_src_dir}
    fi
fi