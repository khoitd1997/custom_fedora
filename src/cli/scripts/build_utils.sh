#!/bin/bash
# storing utils for helping build generation
current_script_dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
source ${current_script_dir}/misc_utils.sh

## build_env_var_file os_name releasever arch clear_cache parser_mode path
build_env_var_file() {
    cat > "$6" << EOF
    export env_os_name="$1"
    export env_parent_config="$1.toml"

    export env_releasever="$2"
    export env_arch="$3"

    export env_clear_cache="$4"
    export env_parser_mode="$5"
EOF
}

## assume that set_env_var.sh has been sourced
build_project_file_structure() {
    if [ -z "${env_base_dir}" ]; then
        print_error "set_env_var.sh has not been sourced"
        exit 1
    fi
    
    mkdir -p ${env_build_dir}
    # TODO(kd): Replace with correct command
    touch ${env_package_list_path}
    touch ${env_group_list_path}
    
    mkdir -p ${env_share_dir}
    mkdir -p ${env_user_supplied_dir}
    mkdir -p ${env_log_dir}
    mkdir -p ${env_repo_dir}
    rm -rf ${env_repo_dir}/*
    cp -r /etc/yum.repos.d/* ${env_repo_dir}
}