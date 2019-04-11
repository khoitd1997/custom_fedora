#!/bin/bash
# install dependencies for a build and prep the base mock env

set -e
source settings.sh
#---------------------------------------------------

# setup build env
mock -r fedora-${env_fedora_ver}-x86_64 --init 
mock -r fedora-${env_fedora_ver}-x86_64 --install lorax-lmc-novirt nano pykickstart git bash genisoimage squashfs-tools nano vim dnf-plugins-core dnf createrepo bat

# disable metadata override
sudo sed -i '/metadata_expire/d' /etc/mock/fedora-${env_fedora_ver}-x86_64.cfg

# enable networking for mock
sudo sed -i -r "/config_opts\['rpmbuild_networking'\]/s/^#//g"  /etc/mock/site-defaults.cfg
sudo sed -i -r "/config_opts\['rpmbuild_networking'\]/s/False/True/g" /etc/mock/site-defaults.cfg
sudo sed -i -r "/config_opts\['rpmbuild_networking'\]/s/^[ \t]*//g" /etc/mock/site-defaults.cfg