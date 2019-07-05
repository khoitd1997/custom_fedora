#!/bin/bash

currDir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

mkdir -p ${currDir}/build
cd ${currDir}/build

# test setup
# TODO(kd): Move this to end-to-end test place
mkdir -p ${currDir}/build/bin/build/logs
mkdir -p ${currDir}/build/googletest-src
mkdir -p ${currDir}/build/googletest-download
mkdir -p ${currDir}/build/googletest-build

cp ${currDir}/../../example/example_settings.toml ${currDir}/build/bin
cp ${currDir}/../../example/child_setting_1.toml ${currDir}/build/bin
mkdir -p ${currDir}/build/bin/child_path
cp ${currDir}/../../example/child_path/child_setting_2.toml ${currDir}/build/bin/child_path
cat > ${currDir}/build/bin/test_first_login.sh <<EOF
cp place_1 place_2
# some comment
ls
EOF

# scan-build cmake -G Ninja .. && scan-build ninja && ./bin/tomlparser ./bin/settings.toml

mkdir -p /build_share
cp -r ${currDir}/assets /build_share

set -e

# exercise the executable
# TODO(kd): Move this to end-to-end test place
cd bin
mkdir -p out
mkdir -p out/logs
touch ${currDir}/build/bin/build/package_list.txt
touch ${currDir}/build/bin/build/group_list.txt
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
export env_base_dir="${currDir}/build/bin" # TODO(kd): remove after
export env_build_dir="${env_base_dir}/build"
export env_script_dir="${env_build_dir}/scripts"

export env_share_dir="/build_share"
export env_stock_kickstart_dir="${env_share_dir}/fedora-kickstarts"
export env_asset_dir="${env_share_dir}/assets"
export env_valid_keyboard_path="${env_asset_dir}/valid_keyboard.txt"
export env_valid_language_path="${env_asset_dir}/valid_language.txt"
export env_valid_timezone_path="${env_asset_dir}/valid_timezone.txt"

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

# export GTEST_FILTER="CommonSanitizeTest_*" # MUST SPECIFY HERE BEFORE THE TESTS ARE DISCOVERED
# GTEST_BREAK_ON_FAILURE=1 GTEST_COLOR=1 ctest --verbose --gtest_print_time=0
cd ${currDir}/build
cmake .. && cmake --build .

cd ${currDir}/build/bin
./tomlparser example_settings.toml
if [ $? -ne 0 ]; then
    echo "toml parser failed"
else
    echo "toml parser succeeded"
    cp build/env_var.sh build/prev_env_var.sh
    sed '/env_/s/env_/prev_env_/' -i build/prev_env_var.sh
fi