# Customizing Gnome after kick start installation

To make sure that your gnome customizations stay after installation:

```shell
# inside kickstart file
%post
# configure theme
# add your customizations between the FOE
cat >> /usr/share/glib-2.0/schemas/99_my_custom_settings.gschema.override << FOE
[org.gnome.desktop.interface]
gtk-theme='Adwaita-dark'
FOE

# this line always needed to update new customizations
glib-compile-schemas /usr/share/glib-2.0/schemas/
%end
```