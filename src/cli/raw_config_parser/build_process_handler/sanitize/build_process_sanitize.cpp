#include "build_process_sanitize.hpp"

#include "common_sanitize.hpp"
#include "raw_config_parser_utils.hpp"

namespace hatter {
namespace build_process_handler {
std::vector<std::shared_ptr<HatterParserError>> sanitize(const BuildProcessConfig& buildConf,
                                                         const toml::table&        table) {
    std::vector<std::shared_ptr<HatterParserError>> errors;

    if (auto error = checkUnknownValue(table)) { errors.push_back(error); }
    if (auto error = checkFileNotExist("custom_mock_script", buildConf.mockScriptPaths.value)) {
        errors.push_back(error);
    }

    return errors;
}
}  // namespace build_process_handler
}  // namespace hatter