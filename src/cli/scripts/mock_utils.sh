#!/bin/bash
# mock related functions for hatter

current_script_dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
source ${current_script_dir}/exit_code.sh
source ${current_script_dir}/set_env_var.sh "/builddir/${os_name}"

# list of bare minimum tools needed to run the bootstrap script as well as core iso build tools
mock_base_tools=" bash dnf lorax-lmc-novirt pykickstart genisoimage livecd-tools dnf-plugins-core nano "

mock_base_cmd="mock -r ${build_working_dir}/mock.cfg -q"
mock_execute_bash_cmd="${mock_base_cmd} --chroot"
mock_copyin_cmd="${mock_base_cmd} --copyin"

clear_mock_env() {
    if [ -f ${mock_cfg_path} ]; then
        ${mock_base_cmd} --orphanskill
        ${mock_base_cmd} --clean
    fi
}

create_mock_env() {
    ${mock_base_cmd} --init
    ${mock_base_cmd} --install ${mock_base_tools}
}

bootstrap_mock_env() {
    # copy build scripts
    # TODO(kd): Reconsider path of executable
    ${mock_execute_bash_cmd} "mkdir -p ${env_asset_dir}"
    ${mock_execute_bash_cmd} "mkdir -p ${env_boostrap_script_dir}"
    ${mock_copyin_cmd} ${current_script_dir}/*.sh ${env_script_dir}
    ${mock_copyin_cmd} ${current_script_dir}/bootstrap/* ${env_boostrap_script_dir}
    ${mock_copyin_cmd} ${current_script_dir}/assets/* ${env_asset_dir}

    # TODO(kd): Remove after test
    cp /home/kd/hatter/src/cli/cmake_build_dir/bin/hatter_config_builder_parser ${current_script_dir}/assets/hatter_config_builder_parser
    ${mock_copyin_cmd} ${current_script_dir}/assets/hatter_config_builder_parser ${env_share_dir}
    
    set +e
    if ! ${mock_execute_bash_cmd} ${env_script_dir}/bootstrap/mock_bootstrap.sh ; then
        print_warn "bootstrap failed the first time, rerunning again"
        set -e
        # this tends to fail the first time on the software install step
        ${mock_execute_bash_cmd} ${env_script_dir}/bootstrap/mock_bootstrap.sh
    fi
    set -e
}

prepare_mock_build() {
    # wipe previous user build stuffs
    ${mock_execute_bash_cmd} "rm -rf ${env_user_supplied_dir}"
    ${mock_execute_bash_cmd} "mkdir -p ${env_user_supplied_dir}"
    
    # copy hatter generated files and files given by user
    ${mock_copyin_cmd} ${input_dir}/* ${env_user_supplied_dir}
    ${mock_copyin_cmd} ${mock_build_file_dir}/* ${env_build_dir}
}

execute_mock_build() {
    ${mock_base_cmd} --old-chroot --cwd=${env_base_dir} --chroot ${env_script_dir}/build_mock.sh
    ${mock_base_cmd} --copyout ${env_out_dir}/*.* ${build_working_dir}/out

    rm -rf ${build_working_dir}/out/log
    ${mock_base_cmd} --copyout ${env_out_dir}/log ${build_working_dir}/out/log
}