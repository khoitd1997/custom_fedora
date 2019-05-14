#include "repo_parser.hpp"

#include <filesystem>
#include <memory>
#include <string>
#include <vector>

#include "toml11/toml.hpp"

#include "toml_utils.hpp"
#include "unknown_value_sanitize.hpp"
#include "utils.hpp"

namespace hatter {
namespace internal {
std::optional<std::shared_ptr<RepoNoLinkError>> checkRepoNoLink(const Repo& repo) {
    if (repo.baseurl.empty() && repo.metaLink.empty()) {
        return std::make_shared<RepoNoLinkError>();
    }
    return {};
}

std::optional<std::shared_ptr<RepoNoGPGKeyError>> checkRepoNoGPGKey(const Repo& repo) {
    if (repo.gpgcheck && repo.gpgkey.empty()) { return std::make_shared<RepoNoGPGKeyError>(); }
    return {};
}
}  // namespace internal

std::string RepoNoLinkError::what() const {
    return "either baseurl or metalink needs to be defined";
}

std::string RepoNoGPGKeyError::what() const {
    return "gpgkey should be defined when gpgcheck is true";
}

std::vector<std::shared_ptr<HatterParserError>> sanitize(const Repo&        repo,
                                                         const toml::table& table) {
    std::vector<std::shared_ptr<HatterParserError>> errors;
    if (auto error = checkUnknownValue(table)) { errors.push_back(*error); }
    if (auto error = internal::checkRepoNoLink(repo)) { errors.push_back(*error); }
    if (auto error = internal::checkRepoNoGPGKey(repo)) { errors.push_back(*error); }

    return errors;
}

std::vector<std::shared_ptr<HatterParserError>> sanitize(const RepoConfig&  repoConf,
                                                         const toml::table& table) {
    // TODO(kd): Finish this
    (void)repoConf;
    std::vector<std::shared_ptr<HatterParserError>> errors;
    if (auto error = checkUnknownValue(table)) { errors.push_back(*error); }

    return errors;
}

std::optional<TopSectionErrorReport> getSection(toml::table& rawConfig, RepoConfig& repoConfig) {
    TopSectionErrorReport errorReport("repo");
    bool                  topHasError = false;

    toml::table rawRepoConfig;
    topHasError |= processError(errorReport, getTOMLVal(rawConfig, "repo", rawRepoConfig));
    if (topHasError || rawRepoConfig.empty()) { return errorReport; }

    topHasError |= processError(
        errorReport, getTOMLVal(rawRepoConfig, "standard_repos", repoConfig.standardRepos));
    topHasError |=
        processError(errorReport, getTOMLVal(rawRepoConfig, "copr_repos", repoConfig.coprRepos));

    // parse custom repos
    std::vector<toml::table> rawCustomRepos;
    topHasError |=
        processError(errorReport, getTOMLVal(rawRepoConfig, "custom_repos", rawCustomRepos));
    for (size_t i = 0; i < rawCustomRepos.size(); ++i) {
        SubSectionErrorReport customRepoReport("custom_repo #" + std::to_string(i + 1));
        auto&                 tempTable      = rawCustomRepos.at(i);
        auto                  customHasError = false;
        Repo                  repo;

        customHasError |=
            processError(customRepoReport, getTOMLVal(tempTable, "name", repo.name, false));
        customHasError |= processError(
            customRepoReport, getTOMLVal(tempTable, "display_name", repo.displayName, false));

        customHasError |=
            processError(customRepoReport, getTOMLVal(tempTable, "metalink", repo.metaLink));
        customHasError |=
            processError(customRepoReport, getTOMLVal(tempTable, "baseurl", repo.baseurl));

        customHasError |=
            processError(customRepoReport, getTOMLVal(tempTable, "gpgcheck", repo.gpgcheck, false));
        customHasError |=
            processError(customRepoReport, getTOMLVal(tempTable, "gpgkey", repo.gpgkey));

        repoConfig.customRepos.push_back(repo);

        if (!customHasError) {
            customHasError |= processError(customRepoReport, sanitize(repo, tempTable));
        }

        topHasError |= processError(errorReport, customRepoReport);
    }

    if (!topHasError) {
        topHasError |= processError(errorReport, sanitize(repoConfig, rawRepoConfig));
    }

    if (topHasError) { return errorReport; }

    return {};
}

std::optional<SectionMergeErrorReport> merge(RepoConfig& resultConf, const RepoConfig& targetConf) {
    SectionMergeErrorReport errorReport("repo");
    auto                    mergeHasError = false;

    appendUniqueVector(resultConf.standardRepos, targetConf.standardRepos);
    appendUniqueVector(resultConf.coprRepos, targetConf.coprRepos);

    auto resRepoCnt = 1;
    for (const auto& resRepo : resultConf.customRepos) {
        auto targetRepoCnt = 1;
        for (const auto& targetRepo : targetConf.customRepos) {
            if (resRepo.name == targetRepo.name) {
                if (resRepo != targetRepo) {
                    mergeHasError |= processError(
                        errorReport,
                        SectionMergeConflictError("custom_repo",
                                                  "repo #" + std::to_string(resRepoCnt),
                                                  "repo #" + std::to_string(targetRepoCnt)));
                }
            }
            ++targetRepoCnt;
        }
        ++resRepoCnt;
    }

    if (!mergeHasError) { appendUniqueVector(resultConf.customRepos, targetConf.customRepos); }

    if (mergeHasError) { return errorReport; }

    return {};
}
}  // namespace hatter