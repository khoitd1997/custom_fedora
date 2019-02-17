#!/bin/bash
# script to be used to build fedora inside mock env

set -e
cd /builddir
source settings.sh
#-----------------------------------------------------

rm -rf /var/lmc # delete old output if there is any

cd fedora-kickstarts

kickstart_name=${1}

ksflatten --config ${kickstart_name}.ks -o flat-${kickstart_name}.ks --version ${build_fedora_ver}

livemedia-creator --ks flat-${kickstart_name}.ks --no-virt \
--resultdir /var/lmc --project ${kickstart_name} --make-iso --volid ${kickstart_name} \
--iso-only --iso-name ${kickstart_name}.iso --releasever ${release_ver} \
--title ${kickstart_name} --macboot