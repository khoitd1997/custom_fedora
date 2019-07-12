#!/bin/bash
# mock related functions for hatter

current_script_dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
source ${current_script_dir}/exit_code.sh

mock_build_tools=" lorax-lmc-novirt nano pykickstart git bash genisoimage squashfs-tools nano vim dnf-plugins-core dnf createrepo cpp bat curl jq ripgrep fedora-workstation-repositories findutils tee "
env_base_dir="/builddir/"${os_name} # root directory of all build folder
env_build_dir=${env_base_dir}/build # build directory once in mock env
env_script_dir=${env_build_dir}/scripts # script directory once in mock env
mock_cfg_path=${build_working_dir}/mock.cfg

clear_mock_env() {
    if [ -f ${mock_cfg_path} ]; then
        mock -r ${mock_cfg_path} --orphanskill
        mock -r ${mock_cfg_path} --clean
    fi
}

bootstrap_mock_env() {
    mock -r ${mock_cfg_path} --init
    mock -r ${mock_cfg_path} --install ${mock_build_tools}
}

prepare_mock_build() {
    # wipe previous build stuffs and generate directories
    mock -r ${mock_cfg_path} --chroot "rm -rf ${env_base_dir}"
    mock -r ${mock_cfg_path} --chroot "mkdir -p ${env_build_dir}/user_supplied"
    mock -r ${mock_cfg_path} --chroot "mkdir -p ${env_base_dir}/out"
    
    # copy build scripts
    # TODO(kd): Move scripts to shared
    mock -r ${mock_cfg_path} --copyin ${current_script_dir}/exit_code.sh ${env_script_dir}
    mock -r ${mock_cfg_path} --copyin ${current_script_dir}/build_mock.sh ${env_script_dir}
    mock -r ${mock_cfg_path} --copyin ${current_script_dir}/misc_utils.sh ${env_script_dir}
    # mock -r ${mock_cfg_path} --copyin ${current_script_dir}/hatter_parser ${env_script_dir}
    
    # copy hatter generated files and files given by user
    mock -r ${mock_cfg_path} --copyin ${input_dir}/* ${env_build_dir}/user_supplied
    mock -r ${mock_cfg_path} --copyin ${mock_build_file_dir}/* ${env_build_dir}
}

execute_mock_build() {
    mock -r ${mock_cfg_path} --old-chroot --cwd=${env_build_dir} --chroot /build_mock.sh
    mock -r ${mock_cfg_path} --copyout ${env_build_dir}/out ${build_working_dir}/out
}