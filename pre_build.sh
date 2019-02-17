#!/bin/bash
# install dependencies for a build and prep the base mock env

set -e
source settings.sh
#---------------------------------------------------
# install necessary programs
sudo dnf install kernel-modules-$(uname -r) mock
sudo usermod -a -G mock ${USER}

# setup build env
mock -r fedora-${env_fedora_ver}-x86_64 --init
mock -r fedora-${env_fedora_ver}-x86_64 --install lorax-lmc-novirt nano pykickstart git bash

# enable networking for mock
sudo sed -i -r "/config_opts\['rpmbuild_networking'\]/s/^#//g"  /etc/mock/site-defaults.cfg
sudo sed -i -r "/config_opts\['rpmbuild_networking'\]/s/False/True/g" /etc/mock/site-defaults.cfg