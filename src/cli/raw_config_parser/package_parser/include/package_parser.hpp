#pragma once

#include <filesystem>
#include <optional>
#include <string>

#include "toml11/toml.hpp"

#include "error_report_section_type.hpp"
#include "hatter_config_type.hpp"

namespace hatter {
struct RepoNoLinkError : public HatterParserError {
    std::string what() const override;
};

struct RepoNoGPGKeyError : public HatterParserError {
    std::string what() const override;
};

std::optional<TopSectionErrorReport> get(toml::table& rawConfig, PackageConfig& pkgConfig);

std::optional<SectionMergeErrorReport> merge(PackageConfig&       resultConf,
                                             const PackageConfig& targetConf);

}  // namespace hatter