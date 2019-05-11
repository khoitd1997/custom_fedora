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
    : HatterParserLogicalError(repo.baseurl.empty() && repo.metaLink.empty()) {}

RepoNoGPGKeyError::RepoNoGPGKeyError(const Repo& repo)
    : HatterParserLogicalError(repo.gpgcheck && repo.gpgkey.empty()) {}

bool HatterParserLogicalError::hasError() const { return hasError_; }
HatterParserLogicalError::HatterParserLogicalError(const bool hasError) : hasError_(hasError) {}
HatterParserLogicalError::HatterParserLogicalError() {}
HatterParserLogicalError::~HatterParserLogicalError() {}

SubSectionErrorReport::SubSectionErrorReport(const std::string& sectionName)
    : sectionName{sectionName} {}
bool SubSectionErrorReport::hasError() const { return hasError_; }
void SubSectionErrorReport::setError(const bool status) { hasError_ = status; }

TopSectionErrorReport::TopSectionErrorReport(const std::string& sectionName)
    : SubSectionErrorReport(sectionName) {}

void processError(TopSectionErrorReport& errorReport, const SubSectionErrorReport& error) {
    if (error.hasError()) {
        errorReport.setError(true);
        errorReport.subSectionErrors.push_back(error);
    }
}

std::string UnknownValueError::what() const {
    auto undefinedStr = strJoin(undefinedVals);
    return "unknown value(s): " + undefinedStr;
}

std::string RepoNoLinkError::what() const {
    return "either baseurl or metalink needs to be defined";
}

std::string RepoNoGPGKeyError::what() const {
    return "gpgkey should be defined when gpgcheck is true";
}

TopSectionErrorReport getSection(toml::table& rawConfig, RepoConfig& repoConfig) {
    TopSectionErrorReport errorReport("repo");

    toml::table rawRepoConfig;
    processError(errorReport, getTOMLVal(rawConfig, "repo", rawRepoConfig));
    if (errorReport.hasError() || rawRepoConfig.empty()) { return errorReport; }

    processError(errorReport,
                 getTOMLVal(rawRepoConfig, "standard_repos", repoConfig.standardRepos));
    processError(errorReport, getTOMLVal(rawRepoConfig, "copr_repos", repoConfig.coprRepos));

    // parse custom repos
    std::vector<toml::table> rawCustomRepos;
    processError(errorReport, getTOMLVal(rawRepoConfig, "custom_repos", rawCustomRepos));
    for (size_t i = 0; i < rawCustomRepos.size(); ++i) {
        SubSectionErrorReport customRepoError("custom_repo #" + std::to_string(i + 1));
        auto&                 tempTable = rawCustomRepos.at(i);
        Repo                  repo;

        processError(customRepoError, getTOMLVal(tempTable, "name", repo.name, false));
        processError(customRepoError,
                     getTOMLVal(tempTable, "display_name", repo.displayName, false));

        processError(customRepoError, getTOMLVal(tempTable, "metalink", repo.metaLink));
        processError(customRepoError, getTOMLVal(tempTable, "baseurl", repo.baseurl));

        processError(customRepoError, getTOMLVal(tempTable, "gpgcheck", repo.gpgcheck, false));
        processError(customRepoError, getTOMLVal(tempTable, "gpgkey", repo.gpgkey));

        repoConfig.customRepos.push_back(repo);

        // logically sanitize the repo
        processError(customRepoError, UnknownValueError(tempTable));
        processError(customRepoError, RepoNoLinkError(repo));
        processError(customRepoError, RepoNoGPGKeyError(repo));

        processError(errorReport, customRepoError);
    }

    processError(errorReport, UnknownValueError(rawRepoConfig));

    return errorReport;
}

SectionMergeConflictError::SectionMergeConflictError(const std::string& keyName,
                                                     const std::string& val1,
                                                     const std::string& val2)
    : keyName(keyName), val1(val1), val2(val2) {}
std::string SectionMergeConflictError::what() const {
    return "conflict in " + keyName + ": " + val1 + " vs " + val2;
}
bool SectionMergeConflictError::hasError() const { return !keyName.empty(); }

SectionMergeErrorReport::SectionMergeErrorReport(const std::string& sectionName)
    : sectionName(sectionName) {}
bool SectionMergeErrorReport::hasError() const { return hasError_; }
void SectionMergeErrorReport::setError(const bool status) { hasError_ = status; }
void processError(SectionMergeErrorReport& errorReport, const SectionMergeConflictError& error) {
    if (error.hasError()) {
        errorReport.setError(true);
        errorReport.errors.push_back(error);
    }
}

SectionMergeErrorReport merge(RepoConfig& resultConf, const RepoConfig& targetConf) {
    SectionMergeErrorReport errorReport("repo");
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

    if (!(errorReport.hasError())) {
        appendUniqueVector(resultConf.customRepos, targetConf.customRepos);
    }

    return errorReport;
}

}  // namespace internal
}  // namespace hatter