#!/bin/bash
# launch at the initial creation of mock env

set -e
source /build_share/scripts/set_env_var.sh
source ${env_script_dir}/misc_utils.sh

# runtime tools
dnf install -y -q curl findutils git jq ripgrep \
                  fedora-workstation-repositories \
                  dnf-plugins-core createrepo cpp \
                  squashfs-tools nano vim

print_info "cloning stock fedora-kickstart repo"
git clone https://pagure.io/fedora-kickstarts.git ${env_stock_kickstart_dir}

# install vscode repo
print_info "installing vscode repo"
rpm --import https://packages.microsoft.com/keys/microsoft.asc
sh -c 'echo -e "[code]\nname=Visual Studio Code\nbaseurl=https://packages.microsoft.com/yumrepos/vscode\nenabled=1\ngpgcheck=1\ngpgkey=https://packages.microsoft.com/keys/microsoft.asc" > /etc/yum.repos.d/vscode.repo'

# install rpmfusion
print_info "installing rpmfusion"
dnf install https://download1.rpmfusion.org/free/fedora/rpmfusion-free-release-$(rpm -E %fedora).noarch.rpm https://download1.rpmfusion.org/nonfree/fedora/rpmfusion-nonfree-release-$(rpm -E %fedora).noarch.rpm -y -q

print_info "refreshing dnf cache"
dnf check-update -y -q