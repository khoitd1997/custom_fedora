#include "toml_section_parser.hpp"

#include <string>
#include <vector>

namespace hatter {
namespace internal {
std::shared_ptr<UnknownValueError> checkUnknownOptions(
    const toml::table& table, const std::vector<std::string>& validOptions) {
    UnknownValueError error;
    auto listContain = [](const std::vector<std::string>& list, const std::string& value) {
        return static_cast<bool>(std::count(list.begin(), list.end(), value));
    };

    for (auto const& [key, val] : table) {
        if (!listContain(validOptions, key)) {
            error.hasError = true;
            error.undefinedVals.push_back(key);
        }
    }

    return std::make_shared<UnknownValueError>(error);
}

std::string strJoin(const std::vector<std::string>& v, const std::string& delimiter = ", ") {
    std::string out;
    if (auto i = v.begin(), e = v.end(); i != e) {
        out += *i++;
        for (; i != e; ++i) out.append(delimiter).append(*i);
    }
    return out;
}

SubSectionErrorReport::SubSectionErrorReport(const std::string& sectionName)
    : sectionName{sectionName} {}

TopSectionErrorReport::TopSectionErrorReport(const std::string& sectionName)
    : SubSectionErrorReport(sectionName) {}

TopSectionErrorReport& TopSectionErrorReport::operator+=(const SubSectionErrorReport& error) {
    if (error.hasError) {
        hasError = true;
        subSectionErrors.push_back(error);
    }
    return *this;
}

std::string UnknownValueError::what() {
    auto undefinedStr = strJoin(undefinedVals);
    return "unknown value(s): " + undefinedStr;
}

TopSectionErrorReport getSection(const toml::table& rawConfig, RepoConfig& repoConfig) {
    TopSectionErrorReport errorReport("repo");

    toml::table rawRepoConfig;
    errorReport += getTOMLVal(rawConfig, "repo", rawRepoConfig);
    if (errorReport.hasError || rawRepoConfig.empty()) { return errorReport; }

    errorReport += getTOMLVal(rawRepoConfig, "standard_repos", repoConfig.standardRepos);
    errorReport += getTOMLVal(rawRepoConfig, "copr_repos", repoConfig.coprRepos);

    // parse custom repos
    std::vector<toml::table> rawCustomRepos;
    errorReport += getTOMLVal(rawRepoConfig, "custom_repos", rawCustomRepos);
    for (size_t i = 0; i < rawCustomRepos.size(); ++i) {
        SubSectionErrorReport customRepoError("custom_repo #" + std::to_string(i + 1));
        Repo                  repo;
        customRepoError += getTOMLVal(rawCustomRepos.at(i), "name", repo.name, false);
        customRepoError +=
            getTOMLVal(rawCustomRepos.at(i), "display_name", repo.displayName, false);

        customRepoError += getTOMLVal(rawCustomRepos.at(i), "metalink", repo.metaLink);
        customRepoError += getTOMLVal(rawCustomRepos.at(i), "baseurl", repo.baseurl);

        customRepoError += getTOMLVal(rawCustomRepos.at(i), "gpgcheck", repo.gpgcheck, false);
        customRepoError += getTOMLVal(rawCustomRepos.at(i), "gpgkey", repo.gpgkey);

        repoConfig.customRepos.push_back(repo);

        const std::vector<std::string> validOptions = {
            "name", "display_name", "metalink", "baseurl", "gpgcheck", "gpgkey"};
        customRepoError += checkUnknownOptions(rawCustomRepos.at(i), validOptions);

        errorReport += customRepoError;
    }

    const std::vector<std::string> validOptions = {"standard_repos", "copr_repos", "custom_repos"};
    errorReport += checkUnknownOptions(rawRepoConfig, validOptions);

    return errorReport;
}

}  // namespace internal
}  // namespace hatter