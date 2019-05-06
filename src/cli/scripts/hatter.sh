#!/bin/bash
# entry point script for hatter
# parse cmdline args and pass them in the form
# of .env file to the program inside mock env

# colors for pretty printing
red='\33[38;5;0196m' # for error
cyan='\033[38;5;087m' # for marking the being of a new sections
green='\033[38;5;154m' # for general messages
reset='\033[0m' # for resetting the color

mock_software_list=" lorax-lmc-novirt nano pykickstart git bash genisoimage squashfs-tools nano vim dnf-plugins-core dnf createrepo cpp bat "
original_user=$(logname)

max_releasever=31

print_message()
{
    printf "\n${1}"
    return 0
}


print_error()
{
    >&2 printf "${red}\nError: ${1}${reset}"
    is_valid_cmd=false
    return 0
}

contains() 
{
    [[ "$1" =~ "$2" ]] && return 0 || return 1
}



supported_arch="i386 x86_64 ppc ppc64 aarch64 arm riscv64"
arch=`uname -m`
case "$arch" in
    i?86) arch=i386 ;;
    x86_64) arch="x86_64" ;;
    ppc) arch="ppc" ;;
    ppc64) arch="ppc64" ;;
    aarch64) arch="aarch64" ;;
    arm) arch="arm" ;;
    riscv64) arch="riscv64" ;;
    *) 
    print_error "Unknown architecture"
    exit 1 
    ;;
esac

output_dir=${PWD}/hatter_build
input_config=""
input_dir=""
is_valid_cmd=true
releasever=$(lsb_release -r -s)

clear_cache=false
parser_mode=false

# define call back when all variables have been defined
function cleanup {
sudo -i -u ${original_user} bash << EOF
# copy logs even in failure case
mock -r ${output_dir}/mock.cfg --copyout /builddir/fedora-kickstarts/*.log ${output_dir}/out
printf "Exitting Build\n"
EOF
sudo setenforce 1
}
# TODO(kd): Reenable later
# trap cleanup EXIT

set -e
set -x
#------------------------------------------------------------------------------

while (( "$#" )); do
    case "$1" in
        -o|--outputdir)
            output_dir=$2
            shift 2
        ;;

        -i|--inputconfig)
            input_config=$2
            if [ ! -f ${input_config} ]; then
                print_error "Input config file doesn't exist"
            else
                if [[ ! ${input_config} =~ \.toml$ ]]; then
                    print_error "Invalid input config file"
                fi
            fi
            shift 2
        ;;

        --clear)
            clear_cache=true
            shift 1
        ;;

        -p)
            parser_mode=true
            shift 1
        ;;

        --releasever)
            releasever=$2
            num_regex='^[0-9]+$'
            if ! [[ $releasever =~ $num_regex ]] ; then
                print_error "Releasever must be a number"
            else
                if [ "$releasever" -gt ${max_releasever} ] ; then
                    print_error "Releasever only support up to ${max_releasever}"
                fi
            fi
            shift 2
        ;;

        --arch)
            arch=$2
            shift 2
        ;;

        --help)
cat << EOF
usage: hatter -i intomlfile [-o outputdir] [-p] [--help][--version]
Build custom fedora iso

where:
    -i, --inputconfig    <intomlfile>       specify path to input toml file
    -o, --outputdir      <outputdir>        specify directory where build result is, default is working_directory/hatter_build
    --releasever         <fedora_version>   fedora version of the image, default to current machine
    --arch               <arch>             architecture of the image, default to current machine
    -p                                      only generate config file but doens't build, good for checking if config is correct and seeing internal of hatter
    --clear                                 clear all build cache
    --help                                  show this help text
    --version                               show program version

For bug reporting please go to:
<https://github.com/khoitd1997/hatter/issues>.
EOF
            exit 0
        ;;

        --version)
cat << EOF
hatter 0.1
Copyright (C) 2019 Khoi Trinh.
License GNU General Public License v3.0 <https://github.com/khoitd1997/hatter/blob/master/LICENSE>.
This is free software: you are free to change and redistribute it.
There is NO WARRANTY, to the extent permitted by law.
EOF
            exit 0
        ;;
    esac
done

if ! contains "${supported_arch}" ${arch}; then
    print_error "Unsupported arch, possible values are: ${supported_arch}"
fi

if [ -z ${input_config} ]; then
    print_error "Input config must be supplied with -i toml_config_file"
else
    # get abs path of the top level toml config file as well as output dir
    input_dir=$(dirname "${input_config}")
    input_dir=$(cd ${input_dir} 2> /dev/null && pwd -P)
    output_dir=$(cd ${output_dir} 2> /dev/null && pwd -P)

    # strip path if any of the main toml config file
    input_config=$(basename "${input_config}")
fi


if [ "${is_valid_cmd}" = false ]; then
    exit 1
fi

mkdir -p ${output_dir}/out

# create directory structure for input to mock env
mock_in_dir=${output_dir}/in
mkdir -p ${mock_in_dir}
mock_build_root=${mock_in_dir}/build_root
mkdir -p ${mock_build_root}

# generate mock cfg file
base_cfg_name="fedora-${releasever}-${arch}.cfg"
cp -v /etc/mock/${base_cfg_name} ${output_dir}/mock.cfg
sed -i -r "/config_opts\['root'\]/c config_opts['root'] = 'hatter_mock'" ${output_dir}/mock.cfg
cat >> ${output_dir}/mock.cfg << EOF
config_opts['rpmbuild_networking'] = True
config_opts['plugin_conf']['ccache_enable'] = True
EOF

# create build root
cp -r ${input_dir}/* ${mock_build_root}

cat > ${mock_in_dir}/.env << EOF
input_config=${input_config}
input_dir=${input_dir}

releasever=${releasever}
arch=${arch}

clear_cache=${clear_cache}
parser_mode=${parser_mode}
EOF

if [ ! -f ${output_dir}/.mock_bootstrapped_done ]; then
    mock -r ${output_dir}/mock.cfg --init 
    mock -r ${output_dir}/mock.cfg --install ${mock_software_list}

    touch ${output_dir}/.mock_bootstrapped_done
fi

mock -r ${output_dir}/mock.cfg --chroot "rm -rf /builddir/"
mock -r ${output_dir}/mock.cfg --chroot "mkdir -p /builddir/"
mock -r ${output_dir}/mock.cfg --copyin ${mock_in_dir}/* /builddir
# mock -r ${output_dir}/mock.cfg --old-chroot --chroot /builddir/build_fedora.sh ${1}

# copy out build result
# mkdir -p ${output_dir}/out
# TODO(kd): Consider changing iso name
# mock -r ${output_dir}/mock.cfg --copyout /var/lmc/out.iso ${output_dir}/out

# for debugging only
# mock -r ${output_dir}/mock.cfg --old-chroot --shell



# [ "$UID" -eq 0 ] || exec sudo "$0" "$@" # request root access if not already
# sudo setenforce 0

# sudo -i -u ${original_user} bash << EOF

# EOF