Must have init run and then install can run

in var/lib/mock is the chroot, clean will remove it

/var/lib/name, name is determined by root var, different chroot won't share same dnf cache

Mock refreshes dnf.conf every invocation

TOML:
Repo list: minimum name(both short and long), enabled=(1 or 0) and baseurl or metalink minimum, gpgcheck and key optional. Modular enabled by default. May have debuginfo and source in the future.

Build: env_version, image_based_on version, os_name

Packagelist: Individual and groups(later handle optional, mandatory etc)
%post script: regular %post as well as logging options
TODO: Also add first boot script
Misc: lang, keyboard, timezone
Base_Kickstart: newest or specific version

In cfg file:
Everything except dnf.conf(will be appended later by repo chosen by user)

Maybe /libexec for all other files and just the main one in /usr/bin

Support classic kickstart file but with additional settings through the TOML file like caching.

## Dependency list

sudo dnf install spdlog
