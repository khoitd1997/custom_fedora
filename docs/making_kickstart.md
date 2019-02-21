# Making Custom Kickstart File

Kickstart files are built layer by layer and each upper layer includes the lower layer. A kickstart file usually looks like this:

```shell
# include lower level file, can be in another dir
%include fedora-kickstarts/fedora-live-cinnamon.ks

# other directives, can override lower layer
timezone US/Pacific

# add packages inside this directive, will be combined with
# to packages on lower layers
# can also remove packages
%packages
git # add git
-gcc # remove gcc
%end

%post
# post install script here
%end
```

The fastest and easiest way is to build on top of the kickstart files of the Fedora project, it's recommended that you create your own repo for the kickstart file, add the [Fedora Kickstart Repo](1) as the gitsubmodule, and have your kickstart file include those kickstart files. The gitsubmodule makes sure that you can easily update from upstream Fedora devs. Out of the kickstart files from Fedora, the most complete ones are usually the live ones, base yours on those if you just want simple desktops.

Adding submodule dependency:

```shell
git clone https://your_own_repo
cd your_own_repo

git submodule add https://pagure.io/fedora-kickstarts
```

Creating your own custom file that depends on it:

```shell
# inside your_own_file.ks

# assuming you want to base on cinnamon
%include fedora-kickstarts/fedora-live-cinnamon.ks
```

If you haven't built your software in the long time, do ``git submodule update`` in your repo to make sure the kickstart files are up-to-date.

[1]: https://pagure.io/fedora-kickstarts