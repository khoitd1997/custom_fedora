#include "toml_section_parser.hpp"

#include <string>
#include <vector>

#include "utils.hpp"

namespace hatter {
namespace internal {
std::shared_ptr<UnknownValueError> checkUnknownValueError(const toml::table& table) {
    UnknownValueError error;
    // auto listContain = [](const std::vector<std::string>& list, const std::string& value) {
    //     return static_cast<bool>(std::count(list.begin(), list.end(), value));
    // };

    if (!table.empty()) {
        for (auto const& [key, val] : table) { error.undefinedVals.push_back(key); }
        error.setError(true);
    }
    return std::make_shared<UnknownValueError>(error);
}

std::shared_ptr<RepoNoLinkError> checkRepoNoLinkError(const Repo& repo) {
    RepoNoLinkError repoError;
    repoError.setError(repo.baseurl.empty() && repo.metaLink.empty());
    return std::make_shared<RepoNoLinkError>(repoError);
}

bool SectionSanitizerError::hasError() const { return hasError_; }
void SectionSanitizerError::setError(const bool error) { hasError_ = error; }
SectionSanitizerError::~SectionSanitizerError() {}

SubSectionErrorReport::SubSectionErrorReport(const std::string& sectionName)
    : sectionName{sectionName} {}
bool SubSectionErrorReport::hasError() const { return hasError_; }

TopSectionErrorReport::TopSectionErrorReport(const std::string& sectionName)
    : SubSectionErrorReport(sectionName) {}

TopSectionErrorReport& TopSectionErrorReport::operator+=(const SubSectionErrorReport& error) {
    if (error.hasError()) {
        hasError_ = true;
        subSectionErrors.push_back(error);
    }
    return *this;
}

std::string UnknownValueError::what() {
    auto undefinedStr = strJoin(undefinedVals);
    return "unknown value(s): " + undefinedStr;
}

std::string RepoNoLinkError::what() { return "either baseurl or metalink needs to be defined"; }

TopSectionErrorReport getSection(const toml::table& rawConfig, RepoConfig& repoConfig) {
    TopSectionErrorReport errorReport("repo");

    toml::table rawRepoConfig;
    errorReport += getTOMLVal(rawConfig, "repo", rawRepoConfig);
    if (errorReport.hasError() || rawRepoConfig.empty()) { return errorReport; }

    errorReport += getTOMLVal(rawRepoConfig, "standard_repos", repoConfig.standardRepos);
    errorReport += getTOMLVal(rawRepoConfig, "copr_repos", repoConfig.coprRepos);

    // parse custom repos
    std::vector<toml::table> rawCustomRepos;
    errorReport += getTOMLVal(rawRepoConfig, "custom_repos", rawCustomRepos);
    for (size_t i = 0; i < rawCustomRepos.size(); ++i) {
        SubSectionErrorReport customRepoError("custom_repo #" + std::to_string(i + 1));
        auto                  tempTable = rawCustomRepos.at(i);
        Repo                  repo;

        customRepoError += getTOMLVal(tempTable, "name", repo.name, false);
        customRepoError += getTOMLVal(tempTable, "display_name", repo.displayName, false);

        customRepoError += getTOMLVal(tempTable, "metalink", repo.metaLink);
        customRepoError += getTOMLVal(tempTable, "baseurl", repo.baseurl);

        customRepoError += getTOMLVal(tempTable, "gpgcheck", repo.gpgcheck, false);
        customRepoError += getTOMLVal(tempTable, "gpgkey", repo.gpgkey);

        repoConfig.customRepos.push_back(repo);

        customRepoError += checkUnknownValueError(tempTable);

        customRepoError += checkRepoNoLinkError(repo);

        errorReport += customRepoError;
    }

    errorReport += checkUnknownValueError(rawRepoConfig);

    return errorReport;
}

}  // namespace internal
}  // namespace hatter