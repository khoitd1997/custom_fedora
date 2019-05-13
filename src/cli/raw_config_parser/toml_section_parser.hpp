#pragma once

#include <map>
#include <memory>
#include <optional>
#include <type_traits>
#include <utility>

#include "error_report_type.hpp"
#include "raw_config_parser.hpp"
#include "toml_utils.hpp"

namespace hatter {
namespace internal {
struct RepoNoLinkError : public HatterParserError {
    std::string what() const override;
};

struct RepoNoGPGKeyError : public HatterParserError {
    std::string what() const override;
};

std::optional<SectionMergeErrorReport> merge(RepoConfig& resultConf, const RepoConfig& targetConf);

std::optional<TopSectionErrorReport> getSection(toml::table& rawConfig, RepoConfig& repoConfig);

std::optional<FileErrorReport> getFile(const std::filesystem::path& filePath,
                                       const std::string&           parentFileName,
                                       FullConfig&                  fullConfig);
}  // namespace internal
}  // namespace hatter