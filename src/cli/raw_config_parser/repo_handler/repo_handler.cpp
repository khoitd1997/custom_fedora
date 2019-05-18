#include "repo_handler.hpp"

#include <filesystem>
#include <memory>
#include <string>
#include <vector>

#include "toml11/toml.hpp"

#include "ascii_code.hpp"
#include "toml_utils.hpp"
#include "unknown_value_sanitize.hpp"
#include "utils.hpp"

namespace hatter {
namespace repo_handler {
namespace {
static const auto kSectionName   = "repo";
static const auto kSectionFormat = ascii_code::kBlue;

std::shared_ptr<RepoNoLinkError> checkRepoNoLink(const CustomRepo& repo) {
    if (repo.baseurl.empty() && repo.metaLink.empty()) {
        return std::make_shared<RepoNoLinkError>();
    }
    return nullptr;
}

std::shared_ptr<RepoNoGPGKeyError> checkRepoNoGPGKey(const CustomRepo& repo) {
    if (repo.gpgcheck && repo.gpgkey.empty()) { return std::make_shared<RepoNoGPGKeyError>(); }
    return nullptr;
}
}  // namespace

std::string RepoNoLinkError::what() const {
    return "either baseurl or metalink needs to be defined";
}

std::string RepoNoGPGKeyError::what() const {
    return "gpgkey should be defined when gpgcheck is true";
}

std::vector<std::shared_ptr<HatterParserError>> sanitize(const CustomRepo&  repo,
                                                         const toml::table& table) {
    std::vector<std::shared_ptr<HatterParserError>> errors;
    if (auto error = checkUnknownValue(table)) { errors.push_back(error); }
    if (auto error = checkRepoNoLink(repo)) { errors.push_back(error); }
    if (auto error = checkRepoNoGPGKey(repo)) { errors.push_back(error); }

    return errors;
}

std::vector<std::shared_ptr<HatterParserError>> sanitize(const RepoConfig&  repoConf,
                                                         const toml::table& table) {
    // TODO(kd): Finish this
    (void)repoConf;
    std::vector<std::shared_ptr<HatterParserError>> errors;
    if (auto error = checkUnknownValue(table)) { errors.push_back(error); }

    return errors;
}

TopSectionErrorReport parse(toml::table& rawConfig, RepoConfig& repoConfig) {
    TopSectionErrorReport errorReport(kSectionName, kSectionFormat);

    toml::table rawRepoConfig;
    processError(errorReport, getTOMLVal(rawConfig, kSectionName, rawRepoConfig));
    if (errorReport || rawRepoConfig.empty()) { return errorReport; }

    processError(errorReport,
                 getTOMLVal(rawRepoConfig, "standard_repos", repoConfig.standardRepos));
    processError(errorReport, getTOMLVal(rawRepoConfig, "copr_repos", repoConfig.coprRepos));

    // parse custom repos
    std::vector<toml::table> rawCustomRepos;

    processError(errorReport, getTOMLVal(rawRepoConfig, "custom_repos", rawCustomRepos));
    for (size_t i = 0; i < rawCustomRepos.size(); ++i) {
        SubSectionErrorReport customRepoReport("custom_repo_" + std::to_string(i + 1));
        auto&                 tempTable = rawCustomRepos.at(i);
        CustomRepo            repo;

        processError(customRepoReport, getTOMLVal(tempTable, "name", repo.name, false));
        processError(customRepoReport,
                     getTOMLVal(tempTable, "display_name", repo.displayName, false));

        processError(customRepoReport, getTOMLVal(tempTable, "metalink", repo.metaLink));

        processError(customRepoReport, getTOMLVal(tempTable, "baseurl", repo.baseurl));

        processError(customRepoReport, getTOMLVal(tempTable, "gpgcheck", repo.gpgcheck, false));

        processError(customRepoReport, getTOMLVal(tempTable, "gpgkey", repo.gpgkey));

        repoConfig.customRepos.push_back(repo);

        if (!customRepoReport) { processError(customRepoReport, sanitize(repo, tempTable)); }

        processError(errorReport, customRepoReport);
    }

    if (!errorReport) { processError(errorReport, sanitize(repoConfig, rawRepoConfig)); }

    return errorReport;
}

SectionMergeErrorReport merge(RepoConfig& resultConf, const RepoConfig& targetConf) {
    SectionMergeErrorReport errorReport(kSectionName, kSectionFormat);

    appendUniqueVector(resultConf.standardRepos, targetConf.standardRepos);
    appendUniqueVector(resultConf.coprRepos, targetConf.coprRepos);

    auto resRepoCnt = 1;
    for (const auto& resRepo : resultConf.customRepos) {
        auto targetRepoCnt = 1;
        for (const auto& targetRepo : targetConf.customRepos) {
            if (resRepo.name == targetRepo.name) {
                if (resRepo != targetRepo) {
                    processError(
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

    if (!errorReport) { appendUniqueVector(resultConf.customRepos, targetConf.customRepos); }

    return errorReport;
}
}  // namespace repo_handler
}  // namespace hatter