#include "config_builder.hpp"

#include "utils.hpp"

namespace hatter {
void buildMockConfig(const BasicConfig& basicConfig, const std::string& buildDir) {
    auto config = std::string{R"(
# build dir
config_opts['root'] = 'hatter-mock'

# architecture and fedora version
config_opts['target_arch'] = 'x86_64'
config_opts['legal_host_arches'] = ('x86_64',)
)"};
    config.append("config_opts['dist'] = 'fc" + basicConfig.imageVersion + "'\n");
    config.append("config_opts['releasever'] = '" + basicConfig.imageVersion + "'\n");

    config.append(R"(
# plugins
config_opts['extra_chroot_dirs'] = [ '/run/lock', ]
config_opts['plugin_conf']['ccache_enable'] = True

# packages and chroot setup
config_opts['package_manager'] = 'dnf'
)");

    writeFile(config, buildDir + "/mock.cfg");
}
}  // namespace hatter