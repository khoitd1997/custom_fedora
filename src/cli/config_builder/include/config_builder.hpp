#ifndef _CONFIG_BUILDER_HPP
#define _CONFIG_BUILDER_HPP

#include <string>
#include <utility>

#include "hatter_config_type.hpp"
namespace hatter {
namespace config_builder {
std::string                         generateIncludeKickstart(const DistroInfo& distroInfo);
std::string                         generatePackageList(const PackageConfig& pkgConfig);
std::string                         generateMisc(const MiscConfig& miscConfig);
std::pair<std::string, std::string> generateRepoList(const RepoConfig& repoConfig);
}  // namespace config_builder
}  // namespace hatter
#endif