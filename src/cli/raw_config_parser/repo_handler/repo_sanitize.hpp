#pragma once

#include <memory>
#include <string>
#include <vector>

#include "error_report_section_type.hpp"
#include "hatter_config_type.hpp"
#include "toml11/toml.hpp"

namespace hatter {
namespace repo_handler {
struct StandardRepoNotSupportedError : public HatterParserError {
    std::vector<std::string> repos;

    std::string what() const override;
};
std::shared_ptr<StandardRepoNotSupportedError> checkStandardRepo(const RepoConfig& repoConf);

struct CoprRepoNotFoundError : public HatterParserError {
    std::vector<std::string> repos;

    std::string what() const override;
};
std::shared_ptr<CoprRepoNotFoundError> checkCoprRepo(const RepoConfig& repoConf);

std::vector<std::shared_ptr<HatterParserError>> sanitize(const RepoConfig&  repoConf,
                                                         const toml::table& table);
}  // namespace repo_handler
}  // namespace hatter