#include "custom_repo_handler.hpp"

#include "ascii_code.hpp"
#include "custom_repo_sanitize.hpp"
#include "toml_utils.hpp"
#include "utils.hpp"

namespace hatter {
namespace custom_repo_handler {
namespace {
const auto kSectionFormat = ascii_code::kMidGreen;
}  // namespace

std::vector<SubSectionErrorReport> parse(std::vector<toml::table> rawCustomRepos,
                                         std::vector<CustomRepo>& customRepos) {
    std::vector<SubSectionErrorReport> errorReports;

    for (size_t i = 0; i < rawCustomRepos.size(); ++i) {
        SubSectionErrorReport errorReport("custom_repo_" + std::to_string(i + 1), kSectionFormat);
        auto&                 tempTable = rawCustomRepos.at(i);
        CustomRepo            repo;

        processError(errorReport, getTOMLVal(tempTable, "name", repo.name, false));
        processError(errorReport, getTOMLVal(tempTable, "display_name", repo.displayName, false));

        processError(errorReport, getTOMLVal(tempTable, "metalink", repo.metaLink));
        processError(errorReport, getTOMLVal(tempTable, "baseurl", repo.baseurl));

        processError(errorReport, getTOMLVal(tempTable, "gpgcheck", repo.gpgcheck, false));
        processError(errorReport, getTOMLVal(tempTable, "gpgkey", repo.gpgkey));

        if (!errorReport) { processError(errorReport, sanitize(repo, tempTable)); }

        if (errorReport) {
            errorReports.push_back(errorReport);
        } else {
            customRepos.push_back(repo);
        }
    }

    return errorReports;
}

std::vector<SectionMergeConflictError> merge(std::vector<CustomRepo>&       result,
                                             const std::vector<CustomRepo>& target) {
    std::vector<SectionMergeConflictError> errors;

    auto resRepoCnt = 1;
    for (const auto& resRepo : result) {
        auto targetRepoCnt = 1;
        for (const auto& targetRepo : target) {
            if (resRepo.name == targetRepo.name) {
                if (resRepo != targetRepo) {
                    errors.push_back(
                        SectionMergeConflictError("custom_repo",
                                                  "repo #" + std::to_string(resRepoCnt),
                                                  "repo #" + std::to_string(targetRepoCnt)));
                }
            }
            ++targetRepoCnt;
        }
        ++resRepoCnt;
    }

    if (errors.empty()) { appendUniqueVector(result, target); }
    return errors;
}
}  // namespace custom_repo_handler
}  // namespace hatter