# Adding more repos to kickstart

By default the kickstart will include the base fedora(and rawhide if you are on the bleeding edge), adding more repos allow you to install more software during the build.

**WARNING: THE ANNACONDA INSTALLER DOESN'T CHECK GPG KEYS OF REPOS DURING THE INSTALL STEPS"

The hard part of adding repos is about finding the correct info, after you got all the details, it's as simple as:

```shell
# inside kickstart file
# --install to make repo available after install
# check manual for other flags
repo --name=repo-name --install --mirrorlist=https://repo_mirror_link
```

## References

These repos list are obtained by intentionally create the wrong url, the response contains all valid repo:

[Fedora Mirror List](1)

[Rpm Fusion List](2)

## Find the necessary information

To add a repons, the bare minimum you need is the name(also called id) of the repos and either the mirrorlist or a base URL. It's a hit or miss process here and you have to guess the url sometimes, I will give a couple of pointers of where to find.

### Directly Inside the /etc/yum.repos.d

**ON PAPER, THIS METHOD DOESN'T WORK**, but it's the easiest so it's worth a shot and it has worked for me(mostly work for third party repos like google chrome and not on stuffs like rpmfusion)

The easiest way to find the necessary information to add a repo is to install the repos on your build system and then go to look at your ```yum.repos.d```(usually located in /etc/yum.repos.d). Inside will be ```.repo``` files that carry information about repos you have installed such as the name and the baseurl, take google chrome for example:

```shell
# inside /etc/yum.repos.d/google-chrome.repo
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

Repos that work this way:

- google chrome
- vscode

### Guessing the mirror using rpm fusion

While the baseurl inside the repos file may work sometimes, most of the time it won't, you will have to find a way to get the links. The method below tends to work well for rpmfusion and official fedora repos(like modular)

RPM fusion repos tend to have a certain format, so it can be easy to guess. The mirror tends to have the form ```https://mirrors.rpmfusion.org/mirrorlist?repo=$1-$releasever&arch=$basearch```, where $1 is the name of the repos, the name can be found inside the ```/etc/yum.repos.d```

Take rpm fusion non free fedora updates repo for example:

```shell
# inside repo file
[rpmfusion-nonfree-updates]
name=RPM Fusion for Fedora $releasever - Nonfree - Updates

# inside the metalink is usually the name, in this case, it is:
# nonfree-fedora-updates-released
metalink=https://mirrors.rpmfusion.org/metalink?repo=nonfree-fedora-updates-released-$releasever&arch=$basearch
enabled=1
enabled_metadata=1
type=rpm-md
```

Combine the name and the skeleton URL, and we have:

``` shell
# inside kickstart file
repo --name=rpmfusion-nonfree-updates --install --mirrorlist=https://mirrors.rpmfusion.org/mirrorlist?repo=nonfree-fedora-updates-released-$releasever&arch=$basearch
```

Repos that work this way:

- rpm fusion free and non free
- rpm fusion nvdia

[1]: https://mirrors.fedoraproject.org/mirrorlist?repo=updates-released-modular-29&arch=x86_64
[2]: https://mirrors.rpmfusion.org/mirrorlist?repo=nonfe-fedora-29&arch=x86_64