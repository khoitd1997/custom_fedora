#!/bin/bash
# used for setting env variables once inside mock env
# assume current working dir as root of the build

export env_base_dir=$(pwd)
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