#!/bin/bash

# copy some files
# this is only for example, you can use "user_files" to specify files to copy
cp -vr /builddir/fedora-kickstarts/user_file /mnt/sysimage/usr/share/
chmod -R a+r+x /mnt/sysimage/usr/share/user_file