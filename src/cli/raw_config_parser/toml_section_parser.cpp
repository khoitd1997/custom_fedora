#include "toml_section_parser.hpp"

#include <filesystem>
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

bool ErrorReport::hasError() const { return hasError_; }
void ErrorReport::setError(const bool status) { hasError_ = status; }

SubSectionErrorReport::SubSectionErrorReport(const std::string& sectionName)
    : sectionName{sectionName} {}
void SubSectionErrorReport::what() const {
    if (tomlErrors.size() > 0 || sanitizerErrors.size() > 0) {
        std::cout << sectionName << ":" << std::endl;

        for (const auto& tomlErr : tomlErrors) { std::cout << tomlErr->what() << std::endl; }
        for (const auto& sanitizeErr : sanitizerErrors) {
            std::cout << sanitizeErr->what() << std::endl;
        }
    }
}

TopSectionErrorReport::TopSectionErrorReport(const std::string& sectionName)
    : SubSectionErrorReport(sectionName) {}
void TopSectionErrorReport::what() const {
    std::cout << "top section " << sectionName << ":" << std::endl;

    for (const auto& sectionErr : subSectionErrors) { sectionErr.what(); }
}

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
void SectionMergeErrorReport::what() const {
    std::cout << "merge error in section " << sectionName << ":" << std::endl;

    for (const auto& mergeErr : errors) { std::cout << mergeErr.what() << std::endl; }
}

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

FileErrorReport::FileErrorReport(const std::string& fileName, const std::string& parentFile)
    : fileName(fileName), parentFile(parentFile) {}
void FileErrorReport::what() const {
    if (sectionErrors.size() > 0 || mergeErrors.size() > 0) {
        std::string includeStr = (parentFile == "") ? ":" : "(included from):" + parentFile;
        std::cout << fileName << includeStr << std::endl;

        for (const auto& sectionError : sectionErrors) { sectionError.what(); }
        for (const auto& mergeError : mergeErrors) { mergeError.what(); }
    }
}

void processError(FileErrorReport& errorReport, const TopSectionErrorReport& error) {
    if (error.hasError()) {
        errorReport.setError(true);
        errorReport.sectionErrors.push_back(error);
    }
}

void processError(FileErrorReport& errorReport, const SectionMergeErrorReport& error) {
    if (error.hasError()) {
        errorReport.setError(true);
        errorReport.mergeErrors.push_back(error);
    }
}

// FileMergeErrorReport merge(FullConfig& resultConf, const FullConfig& targetConf) {}

FileErrorReport getFile(const std::filesystem::path& filePath,
                        const std::string&           parentFileName,
                        FullConfig&                  fullConfig) {
    auto currDirectory = filePath.parent_path().string();
    auto currFileName  = filePath.filename().string();

    // TODO(kd): error handling here
    auto rawConfig = toml::parse(filePath);

    // TODO(kd): error handling here
    std::vector<std::string> includeFiles;
    getTOMLVal(rawConfig, "include_files", includeFiles);
    std::cout << "Include files:" << std::endl << std::endl;

    FileErrorReport fileReport(currFileName, parentFileName);
    processError(fileReport, getSection(rawConfig, fullConfig.repoConfig));

    auto hasMergeError = false;
    for (const auto& childFile : includeFiles) {
        std::cout << "Parsing file:" << childFile << std::endl;

        auto childPath  = (currDirectory == "") ? childFile : currDirectory + "/" + childFile;
        auto childTable = toml::parse(childPath);

        FullConfig childConf;
        auto       childError = getFile(childPath, currFileName, childConf);

        std::cout << "Standard Repos:" << std::endl;
        for (const auto& stdRepo : childConf.repoConfig.standardRepos) {
            std::cout << stdRepo << std::endl;
        }

        if (!hasMergeError && !childError.hasError()) {
            auto mergeError = merge(fullConfig.repoConfig, childConf.repoConfig);
            processError(fileReport, mergeError);

            hasMergeError = mergeError.hasError();
        }

        std::cout << std::endl;
    }

    if (fileReport.hasError()) { fileReport.what(); }

    return fileReport;
}

}  // namespace internal
}  // namespace hatter