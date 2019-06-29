#ifndef _CONFIG_BUILDER_HPP
#define _CONFIG_BUILDER_HPP

#include <string>
#include <utility>

#include "build_variable.hpp"
#include "hatter_config_type.hpp"

namespace hatter {
namespace config_builder {
// std::string                         buildIncludeKickstart(const DistroInfo& distroInfo);
// std::string                         buildPackageList(const PackageConfig& pkgConfig);
// std::string                         buildMisc(const MiscConfig& miscConfig);
// std::pair<std::string, std::string> buildRepoList(const RepoConfig& repoConfig);

void build(const FullConfig&                       currConfig,
           const FullConfig&                       prevConfig,
           const build_variable::CLIBuildVariable& currBuildVar,
           const build_variable::CLIBuildVariable& prevBuildVar);
}  // namespace config_builder
}  // namespace hatter
#endif