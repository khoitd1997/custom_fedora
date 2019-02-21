#!/bin/bash
# build new image, output iso in current folder

set -e
set -x
source settings.sh

function cleanup {
    # copy logs even in failure case
    mock -r fedora-${env_fedora_ver}-x86_64 --copyout /builddir/fedora-kickstarts/*.log ${currDir}/build/
    printf "Exitting Build\n"
    sudo setenforce 1
}
trap cleanup EXIT
#-----------------------------------------------------

currDir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

sudo setenforce 0

mock -r fedora-"${env_fedora_ver}"-x86_64 --chroot "rm -rf /builddir/"
mock -r fedora-"${env_fedora_ver}"-x86_64 --chroot "mkdir -p /builddir/"
mock -r fedora-"${env_fedora_ver}"-x86_64 --copyin ./build_fedora.sh ./settings.sh ./fedora-kickstarts /builddir
mkdir -p ${currDir}/build
mock -r fedora-${env_fedora_ver}-x86_64 --old-chroot --chroot /builddir/build_fedora.sh ${1}

# copy out build result
mock -r fedora-${env_fedora_ver}-x86_64 --copyout /var/lmc/${1}.iso ${currDir}/build

# for debugging only
# mock -r fedora-${env_fedora_ver}-x86_64 --old-chroot --shell
