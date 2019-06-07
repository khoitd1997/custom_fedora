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

    if (repo1.displayName.value != repo2.displayName.value) { out.push_back("display_name"); }
    if (repo1.metaLink.value != repo2.metaLink.value) { out.push_back("metaLink"); }
    if (repo1.baseurl.value != repo2.baseurl.value) { out.push_back("baseurl"); }
    if (repo1.gpgcheck.value != repo2.gpgcheck.value) { out.push_back("gpgcheck"); }
    if (repo1.gpgkey.value != repo2.gpgkey.value) { out.push_back("gpgkey"); }

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

        errorReport.add({getTOMLVal(tempTable, repo.name),
                         getTOMLVal(tempTable, repo.displayName),
                         getTOMLVal(tempTable, repo.metaLink),
                         getTOMLVal(tempTable, repo.baseurl),
                         getTOMLVal(tempTable, repo.gpgcheck),
                         getTOMLVal(tempTable, repo.gpgkey)});

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
            if (resRepo.name.value == targetRepo.name.value) {
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