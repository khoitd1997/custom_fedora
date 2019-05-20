#include "package_sanitize.hpp"

#include "magic_enum.hpp"

#include "ascii_code.hpp"
#include "common_sanitize.hpp"
#include "utils.hpp"

namespace hatter {
namespace package_handler {
std::vector<std::shared_ptr<HatterParserError>> sanitize(const PackageConfig& pkgConf,
                                                         const toml::table&   table) {
    std::vector<std::shared_ptr<HatterParserError>> errors;

    if (auto error = checkUnknownValue(table)) { errors.push_back(error); }

    (void)pkgConf;
    return errors;
}
}  // namespace package_handler
}  // namespace hatter