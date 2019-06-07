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
    errorReport.add({getBaseTable(rawConfig, repoConfig, rawRepoConfig)});
    if (errorReport || rawRepoConfig.empty()) { return errorReport; }

    errorReport.add({getTOMLVal(rawRepoConfig, repoConfig.standardRepos)});
    errorReport.add({getTOMLVal(rawRepoConfig, repoConfig.coprRepos)});

    // parse custom repos
    std::vector<toml::table> rawCustomRepos;
    errorReport.add({getBaseTable(rawRepoConfig, repoConfig, rawCustomRepos)});
    errorReport.add({custom_repo_handler::parse(rawCustomRepos, repoConfig.customRepos)});

    if (!errorReport) { errorReport.add(sanitize(repoConfig, rawRepoConfig)); }

    return errorReport;
}

TopSectionErrorReport merge(RepoConfig& resultConf, const RepoConfig& targetConf) {
    TopSectionErrorReport errorReport(kSectionName, kSectionFormat);

    appendUniqueVector(resultConf.standardRepos.value, targetConf.standardRepos.value);
    appendUniqueVector(resultConf.coprRepos.value, targetConf.coprRepos.value);

    errorReport.add({custom_repo_handler::merge(resultConf.customRepos, targetConf.customRepos)});

    return errorReport;
}
}  // namespace repo_handler
}  // namespace hatter