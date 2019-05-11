#include "toml_section_parser.hpp"

#include <string>
#include <vector>

#include "utils.hpp"

namespace hatter {
namespace internal {
UnknownValueError::UnknownValueError(const toml::table& table) {
    if (!table.empty()) {
        for (auto const& [key, val] : table) { undefinedVals.push_back(key); }
        hasError_ = true;
    }
}

RepoNoLinkError::RepoNoLinkError(const Repo& repo)
    : SectionSanitizerError(repo.baseurl.empty() && repo.metaLink.empty()) {}

RepoNoGPGKeyError::RepoNoGPGKeyError(const Repo& repo)
    : SectionSanitizerError(repo.gpgcheck && repo.gpgkey.empty()) {}

bool SectionSanitizerError::hasError() const { return hasError_; }
void SectionSanitizerError::setError(const bool error) { hasError_ = error; }
SectionSanitizerError::SectionSanitizerError(const bool hasError) : hasError_(hasError) {}
SectionSanitizerError::SectionSanitizerError() {}
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

std::string RepoNoGPGKeyError::what() { return "gpgkey should be defined when gpgcheck is true"; }

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

        // logically sanitize the repo
        customRepoError += UnknownValueError(tempTable);
        customRepoError += RepoNoLinkError(repo);
        customRepoError += RepoNoGPGKeyError(repo);

        errorReport += customRepoError;
    }

    errorReport += UnknownValueError(rawRepoConfig);

    return errorReport;
}

}  // namespace internal
}  // namespace hatter