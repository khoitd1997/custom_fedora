#include "custom_repo_sanitize.hpp"

#include "ascii_code.hpp"
#include "common_sanitize.hpp"
#include "utils.hpp"

namespace hatter {
namespace custom_repo_handler {
std::string RepoNoLinkError::what() const {
    return "either baseurl or metalink needs to be defined";
}
std::shared_ptr<RepoNoLinkError> checkRepoNoLink(const CustomRepo& repo) {
    if (repo.baseurl.empty() && repo.metaLink.empty()) {
        return std::make_shared<RepoNoLinkError>();
    }
    return nullptr;
}

std::string RepoNoGPGKeyError::what() const {
    return formatStr("gpgkey", ascii_code::kImportantWordFormat) + " should be defined when " +
           formatStr("gpgcheck", ascii_code::kImportantWordFormat) + " is true";
}
std::shared_ptr<RepoNoGPGKeyError> checkRepoNoGPGKey(const CustomRepo& repo) {
    if (repo.gpgcheck && repo.gpgkey.empty()) { return std::make_shared<RepoNoGPGKeyError>(); }
    return nullptr;
}

std::vector<std::shared_ptr<HatterParserError>> sanitize(const CustomRepo&  repo,
                                                         const toml::table& table) {
    std::vector<std::shared_ptr<HatterParserError>> errors;
    if (auto error = checkUnknownValue(table)) { errors.push_back(error); }
    if (auto error = checkRepoNoLink(repo)) { errors.push_back(error); }
    if (auto error = checkRepoNoGPGKey(repo)) { errors.push_back(error); }
    // TODO(kd): Add check for gpgkey directory and url validity

    return errors;
}
}  // namespace custom_repo_handler
}  // namespace hatter