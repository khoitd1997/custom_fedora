#include "distro_info_sanitize.hpp"

#include "common_sanitize.hpp"
#include "raw_config_parser_utils.hpp"

namespace hatter {
namespace distro_info_handler {
std::vector<std::shared_ptr<HatterParserError>> sanitize(const DistroInfo&  distroConf,
                                                         const toml::table& table) {
    std::vector<std::shared_ptr<HatterParserError>> errors;

    if (auto error = checkUnknownValue(table)) { errors.push_back(error); }
    if (auto error = checkInvalidValue(distroConf.baseSpin.keyName,
                                       {distroConf.baseSpin.value},
                                       {"cinnamon", "xfce", "kde", "lxqt"})) {
        errors.push_back(error);
    }
    if (distroConf.kickstartTag.value != "latest") {
        if (auto error =
                checkInvalidValue(distroConf.kickstartTag.keyName,
                                  {distroConf.kickstartTag.value},
                                  "curl -sS https://pagure.io/api/0/fedora-kickstarts/git/tags | "
                                  "jq -r '.tags | .[]'")) {
            errors.push_back(error);
        }
    }

    return errors;
}
}  // namespace distro_info_handler
}  // namespace hatter