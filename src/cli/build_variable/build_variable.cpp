#include "build_variable.hpp"

#include <cstdlib>
#include <filesystem>

namespace hatter {
namespace build_variable {
extern const std::string kOSName     = std::string{std::getenv("env_os_name")};
extern const int         kReleasever = std::stoi(std::string{std::getenv("env_releasever")});
extern const std::string kArch       = std::string{std::getenv("env_arch")};

extern const bool kClearCache =
    (std::string{std::getenv("env_clear_cache")} == "true") ? true : false;
extern const bool kParserMode =
    (std::string{std::getenv("env_parser_mode")} == "true") ? true : false;

// path variables
extern const std::filesystem::path kBaseSharedDir      = std::filesystem::path("/build_shared");
extern const std::filesystem::path kFedoraKickstartDir = kBaseSharedDir / "fedora-kickstarts";

extern const std::filesystem::path kBaseWorkingDir = std::filesystem::current_path();
extern const std::filesystem::path kBuildDir       = std::filesystem::current_path() / "build";

// extern const std::filesystem::path kRepoDir = kBuildDir / "repos";  // TODO(kd): remove after
// test
extern const std::filesystem::path kRepoDir         = "/etc/yum.repos.d";
extern const std::filesystem::path kPackageListPath = kBuildDir / "package_list.txt";
extern const std::filesystem::path kGroupListPath   = kBuildDir / "group_list.txt";

extern const std::filesystem::path kBaseOutDir = kBaseWorkingDir / "out";

extern const std::filesystem::path kBaseUserFileDir      = kBuildDir / "user_supplied";
extern const std::filesystem::path kPrevParentConfigPath = kBuildDir / "prev_config.toml";
extern const std::filesystem::path kParentConfigPath =
    kBaseUserFileDir / std::string{std::getenv("env_parent_config")};
}  // namespace build_variable
}  // namespace hatter