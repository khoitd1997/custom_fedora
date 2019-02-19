#!/bin/bash
# build new image, output iso in current folder

set -e
set -x
source settings.sh

function cleanup {
    printf "Exitting Build\n"
    sudo setenforce 1
}
trap cleanup EXIT
#-----------------------------------------------------

sudo setenforce 0

mock -r fedora-"${env_fedora_ver}"-x86_64 --chroot "rm -rf /builddir/"
mock -r fedora-"${env_fedora_ver}"-x86_64 --chroot "mkdir -p /builddir/"
mock -r fedora-"${env_fedora_ver}"-x86_64 --copyin ./build_fedora.sh ./settings.sh ./fedora-kickstarts /builddir
# mock -r fedora-"${env_fedora_ver}"-x86_64 --copyin ./user_file ./build_fedora.sh ./settings.sh ./fedora-kickstarts /builddir
mock -r fedora-${env_fedora_ver}-x86_64 --old-chroot --chroot /builddir/build_fedora.sh ${1}
mock -r fedora-${env_fedora_ver}-x86_64 --copyout /var/lmc/${1}.iso ./

# for debugging only
# mock -r fedora-${env_fedora_ver}-x86_64 --old-chroot --shell
