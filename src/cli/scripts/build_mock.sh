#!/bin/bash
# script to be used to build OS inside mock env

set -e
set -x

# import function and variable
script_dir="$(dirname "$(readlink -f "$0")")"
cd ${script_dir}
source ${script_dir}/exit_code.sh
source ${script_dir}/general_utils.sh
source ${script_dir}/env

# function error_callback {
#     exit ${error_build_failed}
# }
# trap error_callback ERR

cat > /etc/dnf/dnf.conf << 'EOF'
[main]
keepcache=1
debuglevel=2
reposdir=/etc/yum.repos.d/
logfile=/var/log/yum.log
retries=20
obsoletes=1
gpgcheck=1
assumeyes=1
syslog_ident=mock
syslog_device=
install_weak_deps=0
best=1
max_parallel_downloads=10
clean_requirements_on_remove=True
metadata_expire=-1
module_platform_id=platform:f30
EOF

set +e
# sed -i 's|^reposdir=.*|reposdir=/etc/yum.repos.d/|g' /etc/dnf/dnf.conf
# dnf check-update -y -q
# dnf_status=$?
# total_try=0
# while [ "$dnf_status" -eq 1 ] && [ "$total_try" -lt 5 ] ; do
#     dnf check-update -y -q
#     dnf_status=$?
#     let "total_try++"
# done
set -e

# if [ "$total_try" -eq 5 ] ; then 
#     # TODO(kd): Error logging
#     exit 1
# fi

cache_dir=/package_cache
if [ "${env_clear_cache}" = true ]; then
    rm -rf ${cache_dir}
fi
mkdir -p ${cache_dir}
#-----------------------------------------------------

# wipe old build and generate new output file structure
out_dir=${script_dir}/out
rm -rf ${out_dir}
mkdir ${out_dir}

iso_dir=${out_dir}/iso

conf_dir=${out_dir}/conf
mkdir ${conf_dir}

log_dir=${out_dir}/log
mkdir ${log_dir}

cd ${conf_dir}
# launch parser here and generate a file called toml_parsed_env
# source ./toml_parsed_env
# if parser succeed then move on
# mv parser log to log_dir

generated_ks="final_kickstart.ks"
if [ "${env_parser_mode}" = false ]; then

    # TODO(kd): Move ksflatten to c++ parser
    ksflatten --config ${generated_ks}.ks -o flat-${generated_ks}.ks --version F${env_releasever}

    dnf install https://download1.rpmfusion.org/free/fedora/rpmfusion-free-release-$(rpm -E %fedora).noarch.rpm https://download1.rpmfusion.org/nonfree/fedora/rpmfusion-nonfree-release-$(rpm -E %fedora).noarch.rpm -y -q

    dnf copr enable khoitd1997/toml11 --setopt=reposdir=/etc/yum.repos.d/ -q

    if [ "${env_enable_custom_cache}" = true ]; then
    package_list=""
    group_list=""
    raw_package_list=$(awk '/%packages/{flag=1;next}/%end/{flag=0}flag' flat-${generated_ks}.ks)
    createrepo ${cache_dir} -q
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
        dnf groupinstall --downloadonly --downloaddir=${cache_dir} ${group_list} 
        dnf --downloadonly --forcearch=x86_64 --allowerasing --best --downloaddir=${cache_dir} install ${package_list} 
        createrepo ${cache_dir} -q
    fi

    livemedia-creator --ks flat-${generated_ks}.ks --no-virt \
    --resultdir ${iso_dir} --project ${os_name} --make-iso --volid ${os_name} \
    --iso-only --iso-name ${generated_ks}.iso --releasever ${env_releasever} \
    --title ${env_os_name} --macboot
fi