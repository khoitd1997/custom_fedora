# Customizing Gnome after kick start installation

There are multiple methods to customize Gnome, and they are outlined below, sometimes multiple methods need to be combined to get the desired results

## Using dconf at first login

The best method to customize Gnome is to use dconf when the user first logs in, the recommended method is to create a script in ``/etc/profile.d/`` and checks if the first time config has already been done.

The script will have the logged in user variable such as $HOME or ~ so you can customize per person. If inside the script uses sudo, the first timet the user opens a command prompt, they will be prompted admin password and if they entered correctly, the script will proceed.

You can create a file in ``/etc/profile.d/`` during the %post section of the kickstart file.

Example:

```shell
# inside /etc/profile.d/first_login_setup.sh
#!/bin/bash

# check if customizations have been done
if [ ! -f ~/first_login_setup_done ]; then
    dconf write /org/cinnamon/desktop/interface/gtk-theme "'Mint-Y-Dark'"
    touch ~/first_login_setup_done
```

The nice thing about this method is that dconf write and load work fine but for things that dconf can't reach like lightdm, there is another way.

## Making an override file

Gnome allows creating override files that will set things up, however, this method is pretty inconsistent and dconf should be used first, that say, this works well so far for things like lightdm and it allows setting the default env for user even in the kickstart file.

```shell
# inside kickstart file

%post
# create custom file with highest priority(99)
# the format of the file is INI
# you can check other files in /usr/share/glib-2.0/schemas/ for example
cat >> /usr/share/glib-2.0/schemas/99_my_custom_settings.gschema.override << FOE
[x.dm.slick-greeter]
background='/usr/share/user_file/TCP118v1_by_Tiziano_Consonni.jpg'
background-color='#2ceb26'
logo='/usr/share/user_file/login_logo.png'
draw-user-backgrounds=false
draw-grid=true
enable-hidpi='auto'
font-name='Noto Sans 11'
icon-theme-name='Mint-Y-Aqua'
show-hostname=true
theme-name='Mint-Y-Dark-Aqua'
FOE

# compile the new customizations
glib-compile-schemas /usr/share/glib-2.0/schemas/
%end
```