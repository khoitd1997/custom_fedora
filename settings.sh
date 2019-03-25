#!/bin/bash
# store settings variable for script

#------------------ENV SETTINGS-------------------------
env_fedora_ver=29 # the env that the build will be done in

#------------------BUILD SETTINGS-----------------------
build_fedora_ver=29 # version of fedora that the image is based on
os_name=fedora-kd

#------------------VIRTUAL IMAGE SETTINGS-------------------
image_disk_size=20 # image size in Gb
image_ram_size=3500 # ram size in Mb
virtual_sys_type=vdi
let virtual_core=$(nproc)/2 # number of processors to use for vm