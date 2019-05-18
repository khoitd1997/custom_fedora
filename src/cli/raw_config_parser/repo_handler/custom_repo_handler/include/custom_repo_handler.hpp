#pragma once

#include <optional>
#include <string>
#include <vector>

#include "toml11/toml.hpp"

#include "error_report_section_type.hpp"
#include "hatter_config_type.hpp"

namespace hatter {
namespace custom_repo_handler {
struct RepoNoLinkError : public HatterParserError {
    std::string what() const override;
};

struct RepoNoGPGKeyError : public HatterParserError {
    std::string what() const override;
};

std::optional<SubSectionErrorReport> parse(toml::table& rawConfig, CustomRepo& customRepo);

std::optional<std::vector<SectionMergeConflictError>> merge(std::vector<CustomRepo>&       result,
                                                            const std::vector<CustomRepo>& target);
}  // namespace custom_repo_handler
}  // namespace hatter