#!/bin/bash
# script to be used to build fedora inside mock env

set -e
cd /builddir
mkdir -p /package_cache
kickstart_name=${1}
source settings.sh
#-----------------------------------------------------

rm -rf /var/lmc # delete old output if there is any

cd fedora-kickstarts
ksflatten --config ${kickstart_name}.ks -o flat-${kickstart_name}.ks --version F${build_fedora_ver}

sed -i '/metadata_expire/d' /etc/dnf/dnf.conf
printf "\nmetadata_expire=-1\n" >> /etc/dnf/dnf.conf

sed -i '/keepcache/d' /etc/dnf/dnf.conf
printf "\nkeepcache=1\n" >> /etc/dnf/dnf.conf

# TODO: Redo this later
dnf install https://download1.rpmfusion.org/free/fedora/rpmfusion-free-release-$(rpm -E %fedora).noarch.rpm https://download1.rpmfusion.org/nonfree/fedora/rpmfusion-nonfree-release-$(rpm -E %fedora).noarch.rpm -y


# package caching
dnf check-update -y --refresh
package_list=""
group_list=""
raw_package_list=$(awk '/%packages/{flag=1;next}/%end/{flag=0}flag' flat-${kickstart_name}.ks)
createrepo /package_cache/
while read -r ks_line; do
    # parse groups
    if [[ "$ks_line" =~ ^@.* ]]; then
        group_name=$(cut -d "@" -f 2 <<< "$ks_line")
        group_list="${group_list} ${group_name}"

    else
        # check if it's a delete package
        if [[ ! "$ks_line" =~ ^-.* ]]; then
            if [ ! -z "$ks_line" ]; then
                package_list="${package_list} ${ks_line}"
            fi
        fi
    fi
done <<< "$raw_package_list"

# printf ${package_list}
dnf groupinstall --downloadonly --downloaddir=/package_cache/ ${group_list}
dnf --downloadonly --forcearch=x86_64 --allowerasing --best --downloaddir=/package_cache/ install ${package_list} 
createrepo /package_cache/

livemedia-creator --ks flat-${kickstart_name}.ks --no-virt \
--resultdir /var/lmc --project ${os_name} --make-iso --volid ${os_name} \
--iso-only --iso-name ${kickstart_name}.iso --releasever ${build_fedora_ver} \
--title ${os_name} --macboot