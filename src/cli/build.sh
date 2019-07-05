#!/bin/bash

set -eo pipefail
hatter_src_dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
cmake_build_dir=/cmake_build_dir
if [ -z "${IN_HATTER_DOCKER}" ]; then
    cd ${hatter_src_dir}
    
    docker build -t hattertest .
    docker volume create cmake_build_vol
    # SYS_PTRACE because of sanitizer
    docker run --tty --rm -v cmake_build_vol:${cmake_build_dir} --name test_instance --cap-add SYS_PTRACE hattertest
else
    # TODO(kd): Move this to end-to-end test place
    
    dnf copr enable khoitd1997/toml11 -y
    
    test_build_root_dir=${cmake_build_dir}/bin
    mkdir -p ${test_build_root_dir}/build
    cat > ${test_build_root_dir}/build/env_var.sh << 'EOF'
    export env_os_name="example_setting"
    export env_parent_config="${env_os_name}.toml"

    export env_releasever="29"
    export env_arch="x86_64"

    export env_clear_cache=true
    export env_parser_mode=false
EOF
    source ${hatter_src_dir}/scripts/build_utils.sh
    build_env_var_file "example_setting" "29" \
    "x86_64" "false" \
    "false" ${test_build_root_dir}/build/env_var.sh
    
    source ${test_build_root_dir}/build/env_var.sh
    
    cd ${test_build_root_dir} # set pwd to test_build_root_dir
    source ${hatter_src_dir}/scripts/set_env_var.sh
    
    build_project_file_structure
    cp -r ${hatter_src_dir}/assets ${env_share_dir}
    cp -r ${hatter_src_dir}/example/* ${env_user_supplied_dir}
    
    # export GTEST_FILTER="CommonSanitizeTest_*" # MUST SPECIFY HERE BEFORE THE TESTS ARE DISCOVERED
    # GTEST_BREAK_ON_FAILURE=1 GTEST_COLOR=1 ctest --verbose --gtest_print_time=0
    # scan-build cmake -G Ninja .. && scan-build ninja && ./bin/hatter_cli ./bin/settings.toml
    cd ${cmake_build_dir}
    cmake ${hatter_src_dir} && cmake --build .
    
    ./bin/hatter_cli
    if [ $? -ne 0 ]; then
        echo "toml parser failed"
    else
        echo "toml parser succeeded"
        cp ${env_build_dir}/env_var.sh ${env_build_dir}/prev_env_var.sh
        sed '/env_/s/env_/prev_env_/' -i ${env_build_dir}/prev_env_var.sh
    fi
fi