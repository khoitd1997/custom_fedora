#!/bin/bash
# used for cleaning up build env, only used when you think
# something is wrong

set -e
source settings.sh
#---------------------------------------------------

mock -r fedora-${env_fedora_ver}-x86_64 --clean
