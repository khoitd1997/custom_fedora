#!/bin/bash
# small utils not in particular category for hatter

## colors for pretty printing
red='\33[38;5;0196m' # for error
cyan='\033[38;5;087m' # for marking the being of a new sections
green='\033[38;5;154m' # for general messages
reset='\033[0m' # for resetting the color

print_message() {
    printf "\n${1}"
}

print_error() {
    >&2 printf "${red}\nError: ${1}${reset}"
    is_valid_cmd=false
}

contains() {
    [[ "$1" =~ "$2" ]] && return 0 || return 1
}
