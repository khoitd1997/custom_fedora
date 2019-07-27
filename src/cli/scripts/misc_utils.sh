#!/bin/bash
# small utils not in particular category for hatter

## colors for pretty printing
red='\033[38;5;9m' # for error
cyan='\033[38;5;14m' # for marking the being of a new sections
green='\033[38;5;2m' # for general messages
reset='\033[0m' # for resetting the color

print_info() {
    printf "${green}[info]${reset}\t  ${1}\n"
}

print_error() {
    >&2 printf "${red}[error]${reset}\t  ${1}\n"
    is_valid_cmd=false
}

contains() {
    [[ "$1" =~ "$2" ]] && return 0 || return 1
}
