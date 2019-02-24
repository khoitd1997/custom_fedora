#!/bin/bash
# script to be used to build fedora inside mock env

set -e
cd /builddir
source settings.sh
#-----------------------------------------------------

rm -rf /var/lmc # delete old output if there is any
rm -rf /var/tmp/mnt

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
# tar -cvf - /mnt/linux | (cd /var/tmp/ && tar -xf - )
# umount /mnt/linux

# # extract squashfs to get to rootfs
# cd /var/tmp/mnt/linux/LiveOS
# unsquashfs -i squashfs.img 

# # insert user_file into rootfs
# mount -o loop squashfs-root/LiveOS/rootfs.img /mnt/linux
# cp -vr /builddir/fedora-kickstarts/user_file /mnt/linux/usr/share/
# chmod -R a+r+x /mnt/linux/usr/share/user_file
# umount /mnt/linux
# cd /var/tmp/mnt/linux/LiveOS
# rm -rf squashfs.img
# mksquashfs squashfs-root squashfs.img

# # repackage image
# cd /var/tmp/mnt/linux
# mkisofs -allow-limited-size -o ../${kickstart_name}.iso -b isolinux/isolinux.bin -c isolinux/boot.cat -no-emul-boot \
# -boot-load-size 4 -boot-info-table -J -R -V ${os_name} .
# mv -v ../${kickstart_name}.iso /var/lmc/${kickstart_name}.iso