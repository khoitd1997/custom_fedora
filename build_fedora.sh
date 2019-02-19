#!/bin/bash
# script to be used to build fedora inside mock env

set -e
cd /builddir
source settings.sh
#-----------------------------------------------------

rm -rf /var/lmc # delete old output if there is any

cd fedora-kickstarts

kickstart_name=${1}

ksflatten --config ${kickstart_name}.ks -o flat-${kickstart_name}.ks --version F${build_fedora_ver}

livemedia-creator --ks flat-${kickstart_name}.ks --no-virt \
--resultdir /var/lmc --project ${os_name} --make-iso --volid ${os_name} \
--iso-only --iso-name ${kickstart_name}.iso --releasever ${build_fedora_ver} \
--title ${os_name} --macboot

# # extract filesystem to modify it
# mkdir -p /mnt/linux
# mount -o loop /var/lmc/${kickstart_name}.iso /mnt/linux
# cd /mnt/
# tar -cvf - linux | (cd /var/tmp/ && tar -xf - )
# umount /mnt/linux

# # modify files in /var/tmp/linux
# cd /var/tmp/linux
# cp -rv /builddir/user_file . # move user file to iso

# # repackage image
# cd /var/tmp/linux
# mkisofs -o ../${kickstart_name}.iso -b isolinux/isolinux.bin -c isolinux/boot.cat -no-emul-boot \
# -boot-load-size 4 -boot-info-table -J -R -V ${os_name} .
# mv -v ../${kickstart_name}.iso /var/lmc/${kickstart_name}.iso

# cd /
# rm -rf /var/tmp/linux