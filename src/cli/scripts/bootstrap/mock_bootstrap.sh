#!/bin/bash
# launch at the initial creation of mock env

set -e
current_script_dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
source /build_share/scripts/set_env_var.sh
source ${env_script_dir}/misc_utils.sh

# runtime tools
print_info "installing software tools"
source ${current_script_dir}/mock_software_install.sh

print_info "cloning stock fedora-kickstart repo"
rm -rf ${env_stock_kickstart_dir}
git clone https://pagure.io/fedora-kickstarts.git ${env_stock_kickstart_dir}

print_info "setting up shell prompt"

cat > ~/.vimrc << EOF

syntax on
colorscheme delek

EOF

print_info "refreshing dnf cache"
set +e
dnf check-update -y -q || true
dnf check-update -y -q || true
dnf check-update -y -q || true