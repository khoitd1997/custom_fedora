Must have init run and then install can run

in var/lib/mock is the chroot, clean will remove it

/var/lib/name, name is determined by root var, different chroot won't share same dnf cache

Mock refreshes dnf.conf every invocation

Maybe /libexec for all other files and just the main one in /usr/bin

Support classic kickstart file but with additional settings through the TOML file like caching.

## Dependency list

sudo dnf install spdlog

## Build Stages

- First stage: Outside of mock
  - Handle TOML wrong syntax
  - Sanitize command line options
  - Sanitize all sections possible
- Second stage: Inside mock
  - Sanitize packages, repo
  - Control build scripts, enable caching, etc

## Format of generated env file from parser

The file will be called ```toml_parsed_env``` and will contain env variables used for building the final image

Sample:

```shell
# inside toml_parsed_env

env_os_name="fedora-kd"
env_enable_custom_cache=true
```