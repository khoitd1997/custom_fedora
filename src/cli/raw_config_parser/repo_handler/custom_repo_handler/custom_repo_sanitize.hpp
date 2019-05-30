#pragma once

#include <memory>
#include <string>
#include <vector>

#include "toml11/toml.hpp"

#include "error_type.hpp"
#include "hatter_config_type.hpp"

namespace hatter {
namespace custom_repo_handler {
struct RepoNoLinkError : public HatterParserError {
    std::string what() const override;
};
std::shared_ptr<RepoNoLinkError> checkRepoNoLink(const CustomRepo& repo);

struct RepoNoGPGKeyError : public HatterParserError {
    std::string what() const override;
};
std::shared_ptr<RepoNoGPGKeyError> checkRepoNoGPGKey(const CustomRepo& repo);

std::vector<std::shared_ptr<HatterParserError>> sanitize(const CustomRepo&  repo,
                                                         const toml::table& table);
}  // namespace custom_repo_handler
}  // namespace hatter