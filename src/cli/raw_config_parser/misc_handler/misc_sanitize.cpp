#include "misc_sanitize.hpp"

#include "common_sanitize.hpp"
#include "raw_config_parser_utils.hpp"

namespace hatter {
namespace misc_handler {
std::vector<std::shared_ptr<HatterParserError>> sanitize(const MiscConfig&  miscConf,
                                                         const toml::table& table) {
    std::vector<std::shared_ptr<HatterParserError>> errors;

    if (auto error = checkUnknownValue(table)) { errors.push_back(error); }
    if (auto error =
            checkInvalidValue("keyboard", miscConf.keyboard, "localectl list-x11-keymap-layouts")) {
        errors.push_back(error);
    }
    if (auto error = checkInvalidValue("language", miscConf.language, "locale -a")) {
        errors.push_back(error);
    }
    if (auto error =
            checkInvalidValue("timezone",
                              miscConf.timezone,
                              "cd /usr/share/zoneinfo/posix && find * -type f -or -type l")) {
        errors.push_back(error);
    }

    return errors;
}
}  // namespace misc_handler
}  // namespace hatter