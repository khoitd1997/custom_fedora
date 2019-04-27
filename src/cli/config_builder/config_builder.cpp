#include "config_builder.hpp"

#include <spdlog/spdlog.h>
#include <cstdlib>
#include <iostream>

#include "utils.hpp"

namespace hatter {
bool buildMockConfig(const BasicConfig& basicConfig, const std::string& buildDir) {
    spdlog::info("Building mock cfg file");

    // get default cfg file for the fedora version
    const auto mockCfgPath = buildDir + "/mock.cfg";
    const auto baseCfgName =
        "fedora-" + basicConfig.imageVersion + "-" + basicConfig.imageArch + ".cfg";
    const auto copyCommand = "cp /etc/mock/" + baseCfgName + " " + mockCfgPath + " 2>/dev/null";
    auto       ret         = system(copyCommand.c_str());
    if (0 != ret) {
        spdlog::error("Failed to copy base cfg file");
        return false;
    }

    std::vector<std::string> lines;
    if (readFile(lines, mockCfgPath)) {
        replacePattern(
            lines, "\\bconfig_opts\\['root'\\].*", "config_opts['root'] = 'hatter_mock'");
        lines.push_back("config_opts['rpmbuild_networking'] = True");
        lines.push_back("config_opts['plugin_conf']['ccache_enable'] = True");

        writeFile(lines, mockCfgPath);
    } else {
        spdlog::error("Failed to read cfg file");
        return false;
    }
    return true;
}
}  // namespace hatter