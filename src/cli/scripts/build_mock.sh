#!/bin/bash
# script to be used to build fedora inside mock env

printf "check_point"

# set -e
# # set -x

# # import function and variable
# script_dir="$(dirname "$(readlink -f "$0")")"
# source ${script_dir}/exit_code.sh
# source ${script_dir}/general_utils.sh

# function error_callback {
#     exit ${error_build_failed}
# }
# trap error_callback ERR

# cat > /etc/dnf/dnf.conf << 'EOF'
# [main]
# keepcache=1
# debuglevel=2
# reposdir=/etc/yum.repos.d/
# logfile=/var/log/yum.log
# retries=20
# obsoletes=1
# gpgcheck=1
# assumeyes=1
# syslog_ident=mock
# syslog_device=
# install_weak_deps=0
# best=1
# max_parallel_downloads=10
# clean_requirements_on_remove=True
# module_platform_id=platform:f30
# EOF

# # has a chance of failure and need return code so execute before set -e
# # sed -i 's|^reposdir=.*|reposdir=/etc/yum.repos.d/|g' /etc/dnf/dnf.conf
# dnf check-update -y -q
# dnf_status=$?
# total_try=0
# while [ "$dnf_status" -eq 1 ] && [ "$total_try" -lt 5 ] ; do
#     dnf check-update -y -q
#     dnf_status=$?
#     let "total_try++"
# done

# if [ "$total_try" -eq 5 ] ; then 
#     # TODO(kd): Error logging
#     exit 1
# fi

# cd /builddir
# mkdir -p /package_cache
# kickstart_name=${1}
# source settings.sh
# #-----------------------------------------------------

# rm -rf /var/lmc # delete old output if there is any

# cd fedora-kickstarts
# ksflatten --config ${kickstart_name}.ks -o flat-${kickstart_name}.ks --version F${build_fedora_ver}

# # sed -i '/metadata_expire/d' /etc/dnf/dnf.conf
# # printf "\nmetadata_expire=-1\n" >> /etc/dnf/dnf.conf

# # sed -i '/keepcache/d' /etc/dnf/dnf.conf
# # printf "\nkeepcache=1\n" >> /etc/dnf/dnf.conf

# # dnf install -y rpmfusion-free-release-tainted rpmfusion-free-release rpmfusion-nonfree-release rpmfusion-nonfree-release-tainted mock-rpmfusion-nonfree

# dnf install https://download1.rpmfusion.org/free/fedora/rpmfusion-free-release-$(rpm -E %fedora).noarch.rpm https://download1.rpmfusion.org/nonfree/fedora/rpmfusion-nonfree-release-$(rpm -E %fedora).noarch.rpm -y -q
# # cat /etc/yum.repos.d/rpmfusion-* >> /etc/dnf/dnf.conf

# dnf copr enable khoitd1997/toml11 --setopt=reposdir=/etc/yum.repos.d/ -q

# package_list=""
# group_list=""
# raw_package_list=$(awk '/%packages/{flag=1;next}/%end/{flag=0}flag' flat-${kickstart_name}.ks)
# createrepo /package_cache/ -q
# while read -r ks_line; do
#     # parse groups
#     if [[ "$ks_line" =~ ^@.* ]]; then
#         group_name=$(cut -d "@" -f 2 <<< "$ks_line")
#         group_list="${group_list} ${group_name}"

#     else
#         # check if it's a delete package
#         if [[ ! "$ks_line" =~ ^-.* ]]; then
#             if [ ! -z "$ks_line" ]; then
#                 package_list="${package_list} ${ks_line}"
#             fi
#         fi
#     fi
# done <<< "$raw_package_list"

# # printf ${package_list}
# dnf groupinstall --downloadonly --downloaddir=/package_cache/ ${group_list} 
# dnf --downloadonly --forcearch=x86_64 --allowerasing --best --downloaddir=/package_cache/ install ${package_list} 
# createrepo /package_cache/ -q

# livemedia-creator --ks flat-${kickstart_name}.ks --no-virt \
# --resultdir /var/lmc --project ${os_name} --make-iso --volid ${os_name} \
# --iso-only --iso-name ${kickstart_name}.iso --releasever ${build_fedora_ver} \
# --title ${os_name} --macboot