#!/bin/bash
# entry point script for hatter
# parse cmdline args and pass them in the form
# of .env file to the program inside mock env

## importing functions and variables
script_dir="$(dirname "$(readlink -f "$0")")"
source ${script_dir}/misc_utils.sh
source ${script_dir}/build_utils.sh
source ${script_dir}/exit_code.sh

original_user=$(logname)

max_releasever=31

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
        exit ${error_parsing_failed}
    ;;
esac

build_working_dir=""
generate_build_working_dir=false
input_config=""
input_dir=""
is_valid_cmd=true
releasever=$(lsb_release -r -s)

clear_cache=false
clean_build=false
parser_mode=false

## callback, error handler functions
function all_exit_callback {
    sudo setenforce 1
}
# TODO(kd): Reenable later
# trap all_exit_callback EXIT

set +e
#------------------------------------------------------------------------------

# TODO(kd): Enable when done
# [ "$UID" -eq 0 ] || exec sudo "$0" "$@" # request root access if not already
# sudo setenforce 0
# sudo -i -u ${original_user} bash << EOF

while (( "$#" )); do
    case "$1" in
        -o|--outputdir)
            build_working_dir=$2
            if [[ -z "${build_working_dir// }" ]]; then
                print_error "output dir can't be empty"
                exit 1
            fi
            shift 2
        ;;
        
        -i|--inputconfig)
            input_config=$2
            if [[ -z "${input_config// }" ]]; then
                print_error "input config can't be empty"
                exit 1
            fi
            shift 2
        ;;
        
        --clearcache)
            clear_cache=true
            shift 1
        ;;
        
        --clean)
            clean_build=true
            shift 1
        ;;

        --debug)
            debug_mode=true
            shift 1
        ;;
        
        -p)
            parser_mode=true
            shift 1
        ;;
        
        --releasever)
            releasever=$2
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
    -o, --outputdir      <outputdir>        specify directory where build prepare/result is, default is working_directory/<config_file_name>
    --releasever         <fedora_version>   fedora version of the image, default to current machine
    --arch               <arch>             architecture of the image, default to current machine
    -p                                      only generate config file but doens't build, good for checking if config is correct and seeing internal of hatter
    --clean                                 clean all build files
    --clearcache                            clear all build cache
    --debug                                 open a shell to the build env instead
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

num_regex='^[0-9]+$'
if ! [[ $releasever =~ $num_regex ]] ; then
    print_error "Releasever must be a number"
else
    if [ "$releasever" -gt ${max_releasever} ] ; then
        print_error "Releasever only support up to ${max_releasever}"
    fi
fi

if [ -z ${input_config} ]; then
    print_error "Input config must be supplied with -i toml_config_file"
else
    if [ ! -f ${input_config} ]; then
        print_error "Input config file doesn't exist"
        
        elif [[ ! ${input_config} =~ \.toml$ ]]; then
        print_error "Invalid input config file"
        
    else
        # get abs path of the top level toml config file as well as output dir
        input_dir=$(dirname "${input_config}")
        input_dir=$(cd ${input_dir} 2> /dev/null && pwd -P)
        
        # strip to get only name of config file
        input_config=$(basename "${input_config}")
        
        if [ -z ${build_working_dir} ]; then
            build_working_dir=${PWD}/${input_config}
            generate_build_working_dir=true
            elif [ ! -d ${build_working_dir} ]; then
            print_error "Output directory doesn't exist"
        fi
    fi
fi

if [ "${is_valid_cmd}" = false ]; then
    exit ${error_parsing_failed}
fi

# remove relative path component of directory
build_working_dir=$(cd ${build_working_dir} 2> /dev/null && pwd -P)

if [ -d ${build_working_dir} ] && [ "${clean_build}" = true ] ; then
    print_info "Clearing old mock env\n"
    clear_mock_env
    print_info "Removing old build at ${build_working_dir}\n"
    rm -r ${build_working_dir}
fi

if [ "${generate_build_working_dir}" = true ]; then
    mkdir -p ${build_working_dir}
fi

# create directory for storing files necessary for mock build (except for user supplied file)
mock_build_file_dir=${build_working_dir}/build_file
rm -rf ${mock_build_file_dir}
mkdir -p ${mock_build_file_dir}

# generate mock cfg file
base_cfg_name="fedora-${releasever}-${arch}.cfg"
cp /etc/mock/${base_cfg_name} ${build_working_dir}/mock.cfg

# get free loop device to mount in
# TODO(kd): Remove this once the newest version of mock comes out
curr_loop_num=$(sudo losetup -f | grep -Eo '[0-9]+$')
free_loop_device1="/dev/loop${curr_loop_num}"
let curr_loop_num+=1
free_loop_device2="/dev/loop${curr_loop_num}"

sed -i -r "/config_opts\['root'\]/c config_opts['root'] = 'hatter_mock'" ${build_working_dir}/mock.cfg

cat >> ${build_working_dir}/mock.cfg << EOF
config_opts['rpmbuild_networking'] = True
config_opts['plugin_conf']['ccache_enable'] = True

config_opts['plugin_conf']['bind_mount_enable'] = True
config_opts['plugin_conf']['bind_mount_opts']['dirs'].append(('${free_loop_device1}', '${free_loop_device1}' ))
config_opts['plugin_conf']['bind_mount_opts']['dirs'].append(('${free_loop_device2}', '${free_loop_device2}' ))
config_opts['environment']['PS1'] = r'\[\033[38;5;14m\]\w\[$(tput sgr0)\]\[\033[38;5;15m\] \[$(tput sgr0)\]\[\033[38;5;9m\][\$?]\[$(tput sgr0)\] '
EOF

if [ "${debug_mode}" = true ]; then
    mock -r ${build_working_dir}/mock.cfg --old-chroot --shell
    exit 0
fi 

# generate env variable for build script inside mock
os_name="${input_config%.*}"
build_env_var_file ${os_name} \
                   ${releasever} \
                   ${arch} \
                   ${clear_cache} \
                   ${parser_mode} \
                   ${mock_build_file_dir}/env_var.sh

# start doing mock build
source ${script_dir}/mock_utils.sh
# if [ ! -f ${build_working_dir}/.mock_bootstrapped_done ]; then
    print_info "bootrapping build env"
    bootstrap_mock_env
    
    touch ${build_working_dir}/.mock_bootstrapped_done
# fi

print_info "preparing build env"
prepare_mock_build

print_info "starting build"
execute_mock_build
