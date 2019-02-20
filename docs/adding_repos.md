# Adding more repos to kickstart

By default the kickstart will include the base fedora(and rawhide if you are on the bleeding edge), adding more repos allow you to install more software during the build.

**WARNING: THE ANNACONDA INSTALLER DOESN'T CHECK GPG KEYS OF REPOS DURING THE INSTALL STEPS"

## Find the necessary information

To add a repons, the bare minimum you need is the name(also called id) of the repos and either the mirrorlist or a base URL.

The easiest way to find the necessary information to add a repo is to install the repos on your build system and then go to look at your ```yum.repos.d```(usually located in /etc/yum.repos.d). Inside will be ```.repo``` files that carry information about repos you have installed such as the name and the baseurl, you can also get info from ```sudo dnf repolist```, take google chrome for example:

```shell
#inside /etc/yum.repos.d/google-chrome.repo
[google-chrome]
name=google-chrome
baseurl=http://dl.google.com/linux/chrome/rpm/stable/x86_64
enabled=1
gpgcheck=1
gpgkey=https://dl.google.com/linux/linux_signing_key.pub
```

Using those info, add this to your kickstart file, make sure it's not in any section like %post

```shell
# specify name and baseurl, --install means installing the repo and make it persistent
# so that you can use it even after installing on the target computer
repo --name=google-chrome --baseurl=http://dl.google.com/linux/chrome/rpm/stable/x86_64 --install
```

The package for google chrome should be available for installation in kickstart.