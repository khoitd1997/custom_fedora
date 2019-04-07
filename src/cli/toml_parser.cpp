#include "cpptoml.hpp"

// #include <unistd.h>
#include <cassert>
#include <iostream>
#include <stdexcept>
#include <string>

#include <libgen.h>
#include <limits.h>
#include <unistd.h>

// static void checkBuildConfig(const cpptoml::table* buildConfig) {
//     if (!buildConfig->get_as<std::string>("mock_env_fedora_version")){

//     } }

static void buildConfigFile(const cpptoml::table* buildConfig, const std::string& buildDir) {
    auto config = std::string{R"(
# build dir
config_opts['root'] = 'hatter-mock'

# architecture and fedora version
config_opts['target_arch'] = 'x86_64'
config_opts['legal_host_arches'] = ('x86_64',)
)"};
    config.append("config_opts['dist'] = 'fc" +
                  *(buildConfig->get_as<std::string>("image_fedora_version")) + "'\n");
    config.append("config_opts['releasever'] = '" +
                  *(buildConfig->get_as<std::string>("image_fedora_version")) + "'\n");

    config.append(R"(
# plugins
config_opts['extra_chroot_dirs'] = [ '/run/lock', ]
config_opts['plugin_conf']['ccache_enable'] = True

# packages and chroot setup
config_opts['package_manager'] = 'dnf'
)");

    std::ofstream configFile(buildDir + "/mock.cfg", std::ofstream::trunc);
    configFile << config;
    configFile.close();
}

static std::string getExeDir(void) {
    char buff[PATH_MAX];
    auto len = readlink("/proc/self/exe", buff, sizeof(buff) - 1);
    if (len != -1) {
        buff[len] = '\0';
        return std::string(dirname(buff));
    }
    return std::string("");
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " filename" << std::endl;
        return 1;
    }

    std::cout << "Current path is " << getExeDir() << std::endl;
    auto buildDir = getExeDir() + "/build";

    try {
        auto config = cpptoml::parse_file(argv[1]);

        // TODO: add more error checking
        auto buildConfig = config->get_table("build");
        buildConfigFile(buildConfig.get(), buildDir);

    } catch (const cpptoml::parse_exception& e) {
        std::cerr << "Failed to parse " << argv[1] << ": " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
