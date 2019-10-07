#!/bin/bash
# used for installing runtime software for mock build env
set -e
base_dnf_cmd="dnf install -y -q"

${base_dnf_cmd} fedora-workstation-repositories dnf-plugins-core createrepo

${base_dnf_cmd} curl findutils git jq ripgrep \
                cpp squashfs-tools \
                nano vim bat

# install vscode repo
rpm --import https://packages.microsoft.com/keys/microsoft.asc
sh -c 'echo -e "[code]\nname=Visual Studio Code\nbaseurl=https://packages.microsoft.com/yumrepos/vscode\nenabled=1\ngpgcheck=1\ngpgkey=https://packages.microsoft.com/keys/microsoft.asc" > /etc/yum.repos.d/vscode.repo'

# install rpmfusion
${base_dnf_cmd} https://download1.rpmfusion.org/free/fedora/rpmfusion-free-release-$(rpm -E %fedora).noarch.rpm https://download1.rpmfusion.org/nonfree/fedora/rpmfusion-nonfree-release-$(rpm -E %fedora).noarch.rpm