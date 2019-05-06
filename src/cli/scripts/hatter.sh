#!/bin/bash
# entry point script for hatter
# parse cmdline args and pass them in the form
# of .env file to the program inside mock env

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

# colors for pretty printing
red='\33[38;5;0196m' # for error
cyan='\033[38;5;087m' # for marking the being of a new sections
green='\033[38;5;154m' # for general messages
reset='\033[0m' # for resetting the color

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

set -e
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
    # get path of the top level toml config file
    input_dir=$(dirname "${input_config}")
    input_dir=$(cd ${input_dir} 2> /dev/null && pwd -P)

    # strip path if any of the main toml config file
    input_config=$(basename "${input_config}")
fi


if [ "${is_valid_cmd}" = false ]; then
    exit 1
fi
mkdir -p ${output_dir}

touch ${output_dir}/mock.cfg

cat > .env << EOF
input_config=${input_config}
input_dir=${input_dir}

releasever=${releasever}
arch=${arch}

clear_cache=${clear_cache}
parser_mode=${parser_mode}
EOF
