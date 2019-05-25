#include "repo_handler.hpp"

#include <filesystem>
#include <memory>
#include <string>
#include <vector>

#include "toml11/toml.hpp"

#include "custom_repo_handler.hpp"

#include "repo_sanitize.hpp"

#include "toml_utils.hpp"
#include "utils.hpp"

namespace hatter {
namespace repo_handler {
namespace {
static const auto kSectionName   = "repo";
static const auto kSectionFormat = formatter::kErrorTopSectionFormat;
}  // namespace

TopSectionErrorReport parse(toml::table& rawConfig, RepoConfig& repoConfig) {
    TopSectionErrorReport errorReport(kSectionName, kSectionFormat);

    toml::table rawRepoConfig;
    errorReport.add(getTOMLVal(rawConfig, kSectionName, rawRepoConfig));
    if (errorReport || rawRepoConfig.empty()) { return errorReport; }

    errorReport.add(getTOMLVal(rawRepoConfig, "standard_repos", repoConfig.standardRepos));
    errorReport.add(getTOMLVal(rawRepoConfig, "copr_repos", repoConfig.coprRepos));

    // parse custom repos
    std::vector<toml::table> rawCustomRepos;
    errorReport.add(getTOMLVal(rawRepoConfig, "custom_repos", rawCustomRepos));
    errorReport.add(custom_repo_handler::parse(rawCustomRepos, repoConfig.customRepos));

    if (!errorReport) { errorReport.add(sanitize(repoConfig, rawRepoConfig)); }

    return errorReport;
}

TopSectionErrorReport merge(RepoConfig& resultConf, const RepoConfig& targetConf) {
    TopSectionErrorReport errorReport(kSectionName, kSectionFormat);

    appendUniqueVector(resultConf.standardRepos, targetConf.standardRepos);
    appendUniqueVector(resultConf.coprRepos, targetConf.coprRepos);

    errorReport.add(custom_repo_handler::merge(resultConf.customRepos, targetConf.customRepos));

    return errorReport;
}
}  // namespace repo_handler
}  // namespace hatter