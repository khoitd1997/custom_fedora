#include "misc_sanitize.hpp"

#include "build_variable.hpp"
#include "common_sanitize.hpp"
#include "raw_config_parser_utils.hpp"

namespace hatter {
namespace misc_handler {
std::vector<std::shared_ptr<HatterParserError>> sanitize(const MiscConfig&  miscConf,
                                                         const toml::table& table) {
    std::vector<std::shared_ptr<HatterParserError>> errors;

    if (auto error = checkUnknownValue(table)) { errors.push_back(error); }
    if (auto error = checkInvalidValue(miscConf.keyboard.keyName,
                                       std::vector<std::string>{miscConf.keyboard.value},
                                       build_variable::kValidKeyboardPath,
                                       true)) {
        errors.push_back(error);
    }
    if (auto error = checkInvalidValue(miscConf.language.keyName,
                                       {miscConf.language.value},
                                       build_variable::kValidLanguagePath,
                                       true)) {
        errors.push_back(error);
    }
    if (auto error = checkInvalidValue(miscConf.timezone.keyName,
                                       {miscConf.timezone.value},
                                       build_variable::kValidTimezonePath,
                                       true)) {
        errors.push_back(error);
    }

    return errors;
}
}  // namespace misc_handler
}  // namespace hatter