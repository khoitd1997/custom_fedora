#pragma once

#include <filesystem>
#include <optional>
#include <string>

#include "toml11/toml.hpp"

#include "error_report_type.hpp"
#include "hatter_config_type.hpp"

namespace hatter {
struct RepoNoLinkError : public HatterParserError {
    std::string what() const override;
};

struct RepoNoGPGKeyError : public HatterParserError {
    std::string what() const override;
};

std::optional<TopSectionErrorReport> getSection(toml::table& rawConfig, RepoConfig& repoConfig);

std::optional<SectionMergeErrorReport> merge(RepoConfig& resultConf, const RepoConfig& targetConf);

}  // namespace hatter