#!/bin/bash
# used for cleaning the current virt machine

source settings.sh
#-------------------------------------------------------------

if virsh list --all | grep -q ${os_name}; then
    if ! virsh list --inactive | grep -q ${os_name}; then
    virsh destroy ${os_name}
    fi
    virsh undefine ${os_name}
fi