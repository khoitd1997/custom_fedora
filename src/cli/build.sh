#!/bin/bash

currDir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

mkdir -p ${currDir}/build
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

cp ~/hatter/example/example_settings.toml ~/hatter/src/cli/build/bin
cp ~/hatter/example/child_setting_1.toml ~/hatter/src/cli/build/bin
mkdir -p ~/hatter/src/cli/build/bin/child_path
cp ~/hatter/example/child_path/child_setting_2.toml ~/hatter/src/cli/build/bin/child_path
cat > ~/hatter/src/cli/build/bin/test_first_login.sh <<EOF
cp place_1 place_2
# some comment
ls
EOF

# cmake .. && make && ./bin/tomlparser ./bin/example_settings.toml
# scan-build cmake -G Ninja .. && scan-build ninja && ./bin/tomlparser ./bin/settings.toml

set -e
# export GTEST_FILTER="CommonSanitizeTest_*" # MUST SPECIFY HERE BEFORE THE TESTS ARE DISCOVERED
cmake .. && cmake --build .

# GTEST_BREAK_ON_FAILURE=1 GTEST_COLOR=1 ctest --verbose --gtest_print_time=0
cd bin
mkdir -p out
mkdir -p out/logs
set -e
if [ -f "build/prev_env_var.sh" ]; then
    source build/prev_env_var.sh
    export env_is_first_build=false
else
    export env_is_first_build=true
fi
cat > ./build/env_var.sh << 'EOF'
export env_parent_config="fedora_kd.toml"
export env_os_name="fedora_kd"

export env_releasever="29"
export env_arch="x86_64"

export env_clear_cache=true
export env_parser_mode=false

# export env_base_dir="/builddir/${env_os_name}"
export env_base_dir="/home/kd/hatter/src/cli/build/bin" # TODO(kd): remove after
export env_build_dir="${env_base_dir}/build"
export env_script_dir="${env_build_dir}/scripts"

export env_share_dir="/build_shared"
export env_stock_kickstart_dir="${env_share_dir}/fedora-kickstarts"

# export env_repo_dir="env_build_dir/repos"
export env_repo_dir="/etc/yum.repos.d"  # TODO(kd): remove after
export env_package_list_path="${env_build_dir}/package_list.txt"
export env_group_list_path="${env_build_dir}/group_list.txt"

# out
export env_out_dir="${env_base_dir}/out"
export env_main_kickstart_path="${env_out_dir}/${env_os_name}.ks"
export env_first_login_script_path="${env_out_dir}/first_login.sh"
export env_post_build_script_path="${env_out_dir}/post_build.sh"
export env_post_build_script_no_root_path="${env_out_dir}/post_build_no_root.sh"

# log dir
export env_kickstart_log_dir="/root${env_out_dir}/log" # used when specifying log in .ks file
export env_log_dir="${env_out_dir}/log"

export env_user_supplied_dir="${env_build_dir}/user_supplied"
export env_config_builder_env_var_path="${env_build_dir}/config_builder_env_var.sh"
export env_prev_var_path="${env_build_dir}/prev_env_var.sh"
export env_prev_parent_config_path="${env_build_dir}/prev_config.toml"
export env_parent_config_path="${env_user_supplied_dir}/${env_parent_config}"

export env_user_file_dest="/mnt/sysimage/usr/share/hatter_user_file"
EOF
source ./build/env_var.sh
./tomlparser example_settings.toml
if [ $? -ne 0 ]; then
    echo "toml parser failed"
else
    echo "toml parser succeeded"
    cp build/env_var.sh build/prev_env_var.sh
    sed '/env_/s/env_/prev_env_/' -i build/prev_env_var.sh
fi
# cmake -G Ninja .. && ninja && ./bin/tomlparser ./bin/random.toml