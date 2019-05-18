#include "custom_repo_handler.hpp"

namespace hatter {
namespace custom_repo_handler {
std::optional<SubSectionErrorReport> parse(toml::table& rawConfig, CustomRepo& customRepo) {}

std::optional<std::vector<SectionMergeConflictError>> merge(std::vector<CustomRepo>&       result,
                                                            const std::vector<CustomRepo>& target) {
    std::vector<SectionMergeConflictError> errors;

    auto resRepoCnt = 1;
    for (const auto& resRepo : result) {
        auto targetRepoCnt = 1;
        for (const auto& targetRepo : target) {
            if (resRepo.name == targetRepo.name) {
                if (resRepo != targetRepo) {
                    mergeHasError |= processError(
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
}
}  // namespace custom_repo_handler
}  // namespace hatter