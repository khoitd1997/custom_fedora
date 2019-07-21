#!/bin/bash
# used for setting env variables once inside mock env
# if no arg passed when base dir is cwd
# else it is the arg passed in

if [ -z "$1" ]; then
    export env_base_dir=$(pwd)
else
    export env_base_dir="$1"
fi
export env_build_dir="${env_base_dir}/build"
export env_var_path="${env_build_dir}/env_var.sh"
if [ -f ${env_var_path} ]; then
    source ${env_var_path}
fi

# user config related
export env_user_supplied_dir="${env_build_dir}/user_supplied"
export env_config_builder_env_var_path="${env_build_dir}/config_builder_env_var.sh"
export env_prev_var_path="${env_build_dir}/prev_env_var.sh"
export env_prev_parent_config_path="${env_build_dir}/prev_config.toml"
export env_parent_config_path="${env_user_supplied_dir}/${env_parent_config}"
export env_user_file_dest="/mnt/sysimage/usr/share/hatter_user_file"

# shared dir
export env_share_dir="/build_share"
export env_script_dir="${env_share_dir}/scripts"
export env_boostrap_script_dir="${env_script_dir}/bootstrap"
export env_stock_kickstart_dir="${env_share_dir}/fedora-kickstarts"
export env_asset_dir="${env_share_dir}/assets"
export env_valid_keyboard_path="${env_asset_dir}/valid_keyboard.txt"
export env_valid_language_path="${env_asset_dir}/valid_language.txt"
export env_valid_timezone_path="${env_asset_dir}/valid_timezone.txt"

# intermediate build results
export env_base_kickstart_path="${env_build_dir}/flat-base.ks"
export env_base_image_name="base.iso"
export env_base_image_dir="${env_build_dir}/base"
export env_base_image_path="${env_base_image_dir}/${env_base_image_name}"

# out
export env_out_dir="${env_base_dir}/out"
export env_main_kickstart_path="${env_out_dir}/${env_os_name}.ks"
export env_first_login_script_path="${env_out_dir}/first_login.sh"
export env_post_build_script_path="${env_out_dir}/post_build.sh"
export env_post_build_script_no_root_path="${env_out_dir}/post_build_no_root.sh"

# log dir
export env_log_dir="${env_out_dir}/log"
export env_parser_log_path="${env_log_dir}/parser.log"
export env_kickstart_log_path="/root${env_log_dir}/kickstart.log" # used when specifying log in .ks file

# repo and package
export env_repo_dir="${env_build_dir}/repos"
export env_repo_conf="/etc/dnf/dnf.conf"
export env_repo_log="${env_log_dir}/yum.log"
export env_package_list_path="${env_build_dir}/package_list.txt"
export env_group_list_path="${env_build_dir}/group_list.txt"