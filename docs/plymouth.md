# Plymouth customizations for Fedora

Plymouth is the tool that displays the boot and shutdown animations for Linux desktop. The guide below details the steps to customize it.

**WARNING: MAKE SURE THINGS WORK BEFORE TRYING TO SET THE THEME**, there are failsafe but plymouth can mess up your system in some cases.

## References

[Plymouth template](https://github.com/jcklpe/Plymouth-Animated-Boot-Screen-Creator)

[Vlc frame capture](https://www.youtube.com/watch?v=4NuK7wSQUNs)

## Dependencies

You need the script module to run plymouth scripts.

```shell
sudo dnf install plymouth-plugin-script
```

## Get an animations

The original animation can be in any video form, use vlc as shown above to make the video into the frames, it's up to you to pick the sampling ratio and size. After that makes sure that the image is named in continuous name like 1.png, 2.png, 3.png.

## Creating animation and config file

Pick a name for your theme. And create two files called ``{theme_name}.plymouth and animation.script``. The .plymouth file tells the system about your theme and the animation script basically tells the system to display the animation by looping through the pictures. For example if my theme name is called boot.

```ini
# inside boot.plymouth

[Plymouth Theme]
Name=boot
Description=Infinite loop rainbow
ModuleName=script

# change these to where you put your pics and your script
[script]
ImageDir=/usr/share/user_file/plymouth
ScriptFile=/usr/share/user_file/plymouth/animation.script
```

```shell
# inside animation.script
// Copyright 2007 Aslan French <david@jackalope.tech>
//
// The following code is a derivative work of the code from the
// Plymouth-Animated-Boot-Screen-Creator project,
// which is licensed GNUv3. This code therefore is also licensed under the terms
// of the GNU Public License, verison 3.

# Nice colour on top of the screen fading to
Window.SetBackgroundTopColor (0.114, 0.114, 0.114);

# an equally nice colour on the bottom
Window.SetBackgroundBottomColor (0.114, 0.114, 0.114);

# change to total frames pic you have
total_pics = 199
# Image animation loop
for (i = 0; i < total_pics; i++)
  flyingman_image[i] = Image(i + ".png");
flyingman_sprite = Sprite();

# Place animation in the center
flyingman_sprite.SetX(Window.GetWidth() / 2 - flyingman_image[1].GetWidth() / 2);
flyingman_sprite.SetY(Window.GetHeight() / 2 - flyingman_image[1].GetHeight() / 2);

progress = 1;

fun refresh_callback ()
  {
    flyingman_sprite.SetImage(flyingman_image[Math.Int(progress) % total_pics]);
    progress++;
  }

Plymouth.SetRefreshFunction (refresh_callback);
```

Then put your pictures and script where you specified.

Put your .plymouth file in ``/usr/share/plymouth/themes/boot/boot.plymouth`` (assuming that boot is your theme name)

Inside your ``/etc/plymouth/plymouth.conf`` add ``ShowDelay=0``, this makes sure that the animation shows up as soon as possible so you can see it.

## Verify and create new initramfs

```shell
# your theme name should be there along with other system themes
plymouth-set-default-theme --list

# set default theme and recreate the initramfs
sudo plymouth-set-default-theme boot -R
```