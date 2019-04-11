#!/bin/bash
# entry point script for hatter

# colors for pretty printing
red='\33[38;5;0196m' # for error
cyan='\033[38;5;087m' # for marking the being of a new sections
green='\033[38;5;154m' # for general messages
reset='\033[0m' # for resetting the color

original_args=$@
scriptDir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
output_dir=${PWD}
input_config=""
is_valid_cmd=true

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


#------------------------------------------------------------------------------

while (( "$#" )); do
    case "$1" in
        -o|--outputdir)
            output_dir=$2
            if [ ! -d "${output_dir}" ]; then
                print_error "Invalid output directory"
            fi
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
        --help)
cat << EOF
usage: hatter -i intomlfile [-o outputdir] [-p] [--help][--version]
Build custom fedora iso

where:
    -i, --inputconfig <intomlfile>     specify path to input toml file
    -o, --outputdir <outputdir>        specify directory where build directory is made, default is working directory
    -p                                 only generate config file but doens't build, good for checking if config is correct and seeing internal of hatter
    --help                             show this help text
    --version                          show program version

For bug reporting please go to:
<https://github.com/khoitd1997/hatter/issues>.
EOF
            exit 0
        ;;
        --version)
            # TODO(kd): Figure out version mechanism
            print_message "version 0.1"
            exit 0
        ;;
    esac
done

if [ -z ${input_config} ]; then
    print_error "Input config must be supplied with -i toml_config_file"
fi

if [ "${is_valid_cmd}" = false ]; then
    exit 1
fi

# /path/to/cpp $@