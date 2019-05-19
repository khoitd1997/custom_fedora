#include "custom_repo_sanitize.hpp"

#include "common_sanitize.hpp"

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
    return "gpgkey should be defined when gpgcheck is true";
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

    return errors;
}
}  // namespace custom_repo_handler
}  // namespace hatter