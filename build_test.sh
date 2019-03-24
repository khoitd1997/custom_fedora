#!/bin/bash
# scripts for automating virtual machine creation for testing iso
# Usage: build_test.sh path_to_iso
source settings.sh
set -e

currDir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
fs_path=${currDir}/build/${os_name}.${virtual_sys_type}
test_image=${1}
#--------------------------------------------------------------------------
if [ ! -f ${fs_path} ]; then
qemu-img create -f ${virtual_sys_type} ${fs_path} ${image_disk_size}G
fi

if virsh list --all | grep -q ${os_name}; then
    virsh destroy ${os_name}
    virsh undefine ${os_name}
fi

virt-install --name ${os_name} \
--description \"${os_name}\" \
--ram ${image_ram_size} \
--vcpus ${virtual_core} \
--virt-type kvm \
--disk path=${fs_path} \
--os-variant fedora${build_fedora_ver} \
--network default \
--graphics vnc,listen=127.0.0.1,port=5901 \
--cdrom ${test_image} \
--noautoconsole

vncviewer 127.0.0.1:1
virsh shutdown ${os_name}