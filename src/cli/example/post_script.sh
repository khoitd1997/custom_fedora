#!/bin/bash

# example script for post_script
cat >> /etc/profile.d/screen_setup.sh << 'EOF'
total_monitor=$(xrandr -q | grep -w "connected" | wc -l)

if [ "$total_monitor" -eq 2 ];then
    xrandr --output HDMI-0 --left-of DVI-D-0
fi
EOF
chmod a+x /etc/profile.d/screen_setup.sh