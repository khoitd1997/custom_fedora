# Adding more repos to kickstart

By default the kickstart will include the base fedora(and rawhide if you are on the bleeding edge), adding more repos allow you to install more software during the build.

**WARNING: THE ANNACONDA INSTALLER DOESN'T CHECK GPG KEYS OF REPOS DURING THE INSTALL STEPS, SO USE HTTPS"

The hard part of adding repos is about finding the correct info, after you got all the details, it's as simple as:

```shell
# inside kickstart file
# --install to make repo available after install
# check manual for other flags
repo --name=repo-name --install --mirrorlist=https://repo_mirror_link

# or
repo --name=repo-name --install --metalink=https://repo_mirror_link
```

Note that **metalink provides more security and as such should be preferred**.

## References

These repos list are obtained by intentionally create the wrong url, the response contains all valid repo:

[Fedora Mirror List][1]

[Rpm Fusion List][2]

Documentation from Fedora

[Format of Mirror Links][3]

[PyKickstart Docs][4]

## Find the necessary information

To add a repo, the bare minimum you need is the name(also called id) of the repos and either the mirrorlist or a base URL. It's a hit or miss process here and you have to guess the url sometimes, I will give a couple of pointers of where to find.

### Directly Inside the /etc/yum.repos.d

**ON PAPER, THIS METHOD DOESN'T WORK**, but it's the easiest so it's worth a shot and it has worked for me most of the time.

The easiest way is when the repo file provides a meta link, take the fedora main repo for example:

```shell
# inside /etc/yum.repos.d/fedora.repo

[fedora]
name=Fedora $releasever - $basearch
failovermethod=priority
#baseurl=http://download.fedoraproject.org/pub/fedora/linux/releases/$releasever/Everything/$basearch/os/
metalink=https://mirrors.fedoraproject.org/metalink?repo=fedora-$releasever&arch=$basearch
enabled=1
```

Simply grab the metalink and put it in your kickstart, the name is usually the .repo file name:

```shell
repo --name=fedora --metalink=https://mirrors.fedoraproject.org/metalink?repo=fedora-$releasever&arch=$basearch --install
```

If for some reasons, metalink doesn't work or wasn't provided(common for third party repos), use the baseurl or mirror list method:

Again, look at your ``yum.repos.d``(usually located in /etc/yum.repos.d). Inside will be ``.repo`` files that carry information about repos you have installed such as the name and the baseurl, take google chrome for example:

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

[1]: https://mirrors.fedoraproject.org/mirrorlist?repo=updates-released-modular-29&arch=x86_64
[2]: https://mirrors.rpmfusion.org/mirrorlist?repo=nonfe-fedora-29&arch=x86_64
[3]: https://fedoraproject.org/wiki/Infrastructure/MirrorManager
[4]: https://media.readthedocs.org/pdf/pykickstart/latest/pykickstart.pdf