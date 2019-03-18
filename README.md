# Hatter

Hatter is a tool used for automating Custom Fedora OS build.

## References

[Fedora Mock Tutorial](https://fedoraproject.org/wiki/Livemedia-creator-_How_to_create_and_use_a_Live_CD?fbclid=IwAR0ghE6C136ATschv_J9OSWIRHqCp5mxTXvrNPLcZ_p82EHW_thuEJY_oB0)

[Fedora Iso Build Instruction](https://docs.fedoraproject.org/en-US/remix-building/remix-ci/?fbclid=IwAR2ucku-HsEbKXy4H2K5h22kk2wU9-WIrgWvnRVsrxNf38Sk2PSvRgjZTbs)

[Mock Command Options](https://linux.die.net/man/1/mock)

[Fedora Project Kickstart Files](https://pagure.io/fedora-kickstarts)

[Kickstart File Docs](https://pykickstart.readthedocs.io/en/latest/kickstart-docs.html?fbclid=IwAR3AwOKBMyrQxXm72itM4LSx2H9mnuTIaLIzF3yDvwRrwYfpiAWEjgeinJY#chapter-4-pre-installation-script)

[Live Media Creator Docs](https://weldr.io/lorax/livemedia-creator.html)

## How to Customize DE and Other Things in kickstart file

[Gnome](./docs/gnome_customization.md)

[Adding Software Repos](./docs/adding_repos)

[Making Your Own Kickstart](./docs/making_kickstart)

[Plymouth Customization](./docs/plymouth)

## Prereq

This assumes that you are running an RPM-based system like Fedora and use dnf for package management, depending how old your software is, the choices for the mock build env may be limited since newer mock has newer cfg file for newer OS.

This also assumes that you have sudo privilege on your system as well as sufficient space for a new build(probably around 10Gb at worst case).

## Usage

The sections below detail the steps to be followed, some of them may need to be done only once.

### Configure build and image

The ``settings.sh`` contains configurations for both the build env and the image, change it before doing anything else.

For a custom image, you will need your own unflattened kickstart(.ks) file, it's advisable to customize pre-made ks file like [here][4](use the fedora-live-**.ks file for the most complete), there must be a folder called ``fedora-kickstarts`` where your main kickstart file and its dependencies reside. Check the [kickstart file docs][5] for how to customize.

A good strategy is to clone the [fedora-kickstarts repo][4] into the same dir as this repo and then modify it in there.

You can also put all the files you want to include into a folder inside fedora-kickstarts named user_file, it will be available at /usr/share/user_file on the installed distro.

For more info and workflow check the documentation above about making your own kickstart.

```shell
cd custom_fedora
nano settings.sh # customize settings
git clone https://pagure.io/fedora-kickstarts fedora-kickstarts

cd fedora-kickstarts
mkdir user_file
cp file_1 user_file
# replace f29 with fedora version you want to base on
# use tags if you want to be sure
git checkout tags/0.29.2
nano my_own_config.ks
```

### Set up build env

``pre_build.sh`` will install the necessary programs as well as init the basic mock env.

The mock environment is reused after the first setup so if you ever change it(like moving from fedora 29 to fedora 30, change ``settings.sh`` and rerun)

```shell
./pre_build.sh
```

### Build

**DURING BUILD SELINUX WILL ENTER PERMISSIVE MODE**, so make sure to prepare accordingly.

There might be some warnings but if the build keeps going then it should be fine.

The ``build.sh`` script is used to create a new build and will output an iso file and build logs in the ``build`` folder. Pass in the name of the unflattened target ks file without extension, the name of the output iso will be the same as the ks file.

The build will take quite some time depending on your machine, there will be moments where it feels like the program freezes(low CPU usage and nothing seems to move), just wait patiently and if after like 3 hours and it still stays like that then it probably really froze.

For example, to build the live fedora cinnamon iso:

```shell
./build.sh fedora-live-cinnamon
```

### Clean Build

If you think there is something wrong with the mock env, run the ``build_clean`` script and the ``pre_build`` script again.

```shell
./build_clean.sh
./pre_build.sh
```