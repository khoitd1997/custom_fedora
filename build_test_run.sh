#!/bin/bash
# run the virtual machine after installation
# will eject media and then run
source settings.sh
#-------------------------------------------------------------

# may need to change disk name
virsh change-media ${os_name} sda --eject
virsh start ${os_name}
vncviewer 127.0.0.1:1
if ! virsh list --inactive | grep -q ${os_name}; then
virsh shutdown ${os_name}
fi