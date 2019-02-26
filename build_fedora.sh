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