#!/bin/bash
# mock related functions for hatter

script_dir="$(dirname "$(readlink -f "$0")")"
source ${script_dir}/exit_code.sh

mock_software_list=" lorax-lmc-novirt nano pykickstart git bash genisoimage squashfs-tools nano vim dnf-plugins-core dnf createrepo cpp bat "
mock_env_build_dir="/build_dir" # build directory once in mock env
mock_cfg_path=${build_working_dir}/mock.cfg

clear_mock_env() {
sudo -i -u ${original_user} bash << EOF
    if [ -f ${mock_cfg_path} ]; then
        mock -r ${mock_cfg_path} --orphanskill
        mock -r ${mock_cfg_path} --clean
    fi
EOF
}

bootstrap_mock_env() {
    mock -r ${mock_cfg_path} --init
    mock -r ${mock_cfg_path} --install ${mock_software_list}
}

prepare_mock_build() {
    # wipe previous build stuffs
    mock -r ${mock_cfg_path} --chroot "rm -rf ${mock_env_build_dir}"
    mock -r ${mock_cfg_path} --chroot "mkdir ${mock_env_build_dir}"
    mock -r ${mock_cfg_path} --chroot "mkdir ${mock_env_build_dir}/user_supplied"

    # copy build scripts
    mock -r ${mock_cfg_path} --copyin ${script_dir}/exit_code.sh ${mock_env_build_dir}
    mock -r ${mock_cfg_path} --copyin ${script_dir}/build_mock.sh ${mock_env_build_dir}
    mock -r ${mock_cfg_path} --copyin ${script_dir}/general_utils.sh ${mock_env_build_dir}
    
    # copy hatter generated files and files given by user
    mock -r ${mock_cfg_path} --copyin ${input_dir}/* ${mock_env_build_dir}/user_supplied
    mock -r ${mock_cfg_path} --copyin ${mock_build_file_dir}/* ${mock_env_build_dir}
}

execute_mock_build() {
    mock -r ${mock_cfg_path} --old-chroot --chroot ${mock_env_build_dir}/build_mock.sh
    mock -r ${mock_cfg_path} --copyout ${mock_env_build_dir}/out ${build_working_dir}/out
}