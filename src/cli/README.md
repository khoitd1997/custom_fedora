Must have init run and then install can run

in var/lib/mock is the chroot, clean will remove it

/var/lib/name, name is determined by root var, different chroot won't share same dnf cache

Mock refreshes dnf.conf every invocation

Maybe /libexec for all other files and just the main one in /usr/bin

Support classic kickstart file but with additional settings through the TOML file like caching.

## Dependency list

```shell
# library
sudo dnf install spdlog

# for build tool
sudo dnf install cppcheck ninja-build
```

## Build Stages

- First stage: Outside of mock
  - Handle TOML wrong syntax
  - Sanitize command line options
  - Sanitize all sections possible
- Second stage: Inside mock
  - Sanitize packages, repo
  - Control build scripts, enable caching, etc
- Commandline argument:
  - Rebuild flag
  - Clear cache flag
  - Parse/Sanitize only
  - Build output dir
  - Config file/dir
  - Fedora version(default current system)
  - Fedora arch(default current arch)