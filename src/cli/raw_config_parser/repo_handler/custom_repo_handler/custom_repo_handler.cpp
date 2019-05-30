#include "custom_repo_handler.hpp"

#include <vector>

#include "custom_repo_sanitize.hpp"
#include "toml_utils.hpp"
#include "utils.hpp"

namespace hatter {
namespace custom_repo_handler {
namespace {
std::vector<std::string> findConflictRepoElement(const CustomRepo& repo1, const CustomRepo& repo2) {
    std::vector<std::string> out;

    if (repo1.displayName != repo2.displayName) { out.push_back("display_name"); }
    if (repo1.metaLink != repo2.metaLink) { out.push_back("metaLink"); }
    if (repo1.baseurl != repo2.baseurl) { out.push_back("baseurl"); }
    if (repo1.gpgcheck != repo2.gpgcheck) { out.push_back("gpgcheck"); }
    if (repo1.gpgkey != repo2.gpgkey) { out.push_back("gpgkey"); }

    return out;
}
}  // namespace
std::vector<SubSectionErrorReport> parse(std::vector<toml::table> rawCustomRepos,
                                         std::vector<CustomRepo>& customRepos) {
    std::vector<SubSectionErrorReport> errorReports;

    for (size_t i = 0; i < rawCustomRepos.size(); ++i) {
        SubSectionErrorReport errorReport("custom_repo_" + std::to_string(i + 1));
        auto&                 tempTable = rawCustomRepos.at(i);
        CustomRepo            repo;

        errorReport.add({getTOMLVal(tempTable, "name", repo.name, false)});
        errorReport.add({getTOMLVal(tempTable, "display_name", repo.displayName, false)});

        errorReport.add({getTOMLVal(tempTable, "metalink", repo.metaLink)});
        errorReport.add({getTOMLVal(tempTable, "baseurl", repo.baseurl)});

        errorReport.add({getTOMLVal(tempTable, "gpgcheck", repo.gpgcheck, false)});
        errorReport.add({getTOMLVal(tempTable, "gpgkey", repo.gpgkey)});

        if (!errorReport) { errorReport.add(sanitize(repo, tempTable)); }

        if (errorReport) {
            errorReports.push_back(errorReport);
        } else {
            customRepos.push_back(repo);
        }
    }

    return errorReports;
}

SubSectionErrorReport merge(std::vector<CustomRepo>&       result,
                            const std::vector<CustomRepo>& target) {
    SubSectionErrorReport errorReport("custom_repo");

    auto resRepoCnt = 1;
    for (const auto& resRepo : result) {
        auto targetRepoCnt = 1;
        for (const auto& targetRepo : target) {
            if (resRepo.name == targetRepo.name) {
                if (resRepo != targetRepo) {
                    const auto list = findConflictRepoElement(resRepo, targetRepo);
                    for (const auto& key : list) {
                        errorReport.add({std::make_shared<SectionMergeConflictError>(
                            key,
                            "repo #" + std::to_string(resRepoCnt),
                            "repo #" + std::to_string(targetRepoCnt))});
                    }
                }
            }
            ++targetRepoCnt;
        }
        ++resRepoCnt;
    }

    if (!errorReport) { appendUniqueVector(result, target); }
    return errorReport;
}
}  // namespace custom_repo_handler
}  // namespace hatter