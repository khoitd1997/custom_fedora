#pragma once

#include <string>

#include "toml11/toml.hpp"

#include "error_report_section_type.hpp"
#include "hatter_config_type.hpp"

namespace hatter {
namespace repo_handler {
struct RepoNoLinkError : public HatterParserError {
    std::string what() const override;
};

struct RepoNoGPGKeyError : public HatterParserError {
    std::string what() const override;
};

TopSectionErrorReport parse(toml::table& rawConfig, RepoConfig& repoConfig);

SectionMergeErrorReport merge(RepoConfig& resultConf, const RepoConfig& targetConf);

}  // namespace repo_handler
}  // namespace hatter