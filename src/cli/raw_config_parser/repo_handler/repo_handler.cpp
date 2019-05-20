#include "repo_handler.hpp"

#include <filesystem>
#include <memory>
#include <string>
#include <vector>

#include "toml11/toml.hpp"

#include "custom_repo_handler.hpp"

#include "repo_sanitize.hpp"

#include "ascii_code.hpp"
#include "toml_utils.hpp"
#include "utils.hpp"

namespace hatter {
namespace repo_handler {
namespace {
static const auto kSectionName   = "repo";
static const auto kSectionFormat = ascii_code::kErrorLocationSecondLevelFormat;
}  // namespace

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
    processError(errorReport, custom_repo_handler::parse(rawCustomRepos, repoConfig.customRepos));

    if (!errorReport) { processError(errorReport, sanitize(repoConfig, rawRepoConfig)); }

    return errorReport;
}

SectionMergeErrorReport merge(RepoConfig& resultConf, const RepoConfig& targetConf) {
    SectionMergeErrorReport errorReport(kSectionName, kSectionFormat);

    appendUniqueVector(resultConf.standardRepos, targetConf.standardRepos);
    appendUniqueVector(resultConf.coprRepos, targetConf.coprRepos);

    processError(errorReport,
                 custom_repo_handler::merge(resultConf.customRepos, targetConf.customRepos));

    return errorReport;
}
}  // namespace repo_handler
}  // namespace hatter