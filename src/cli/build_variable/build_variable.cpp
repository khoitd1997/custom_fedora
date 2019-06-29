#include "build_variable.hpp"

#include <cstdlib>
#include <exception>
#include <filesystem>

namespace hatter {
namespace build_variable {
namespace {
std::string getEnv(const std::string& envVar) {
    auto res = std::getenv(envVar.c_str());
    if (res) {
        return std::string{res};
    } else {
        throw std::runtime_error("failed to get environment variable " + envVar);
    }
}
}  // namespace

CLIBuildVariable::CLIBuildVariable(const std::string& prefix) {
    const std::string fullPrefix = prefix.empty() ? "" : prefix + "_";

    osName     = getEnv(fullPrefix + "env_os_name");
    releasever = std::stoi(getEnv(fullPrefix + "env_releasever"));
    arch       = getEnv(fullPrefix + "env_arch");
    parserMode = (getEnv(fullPrefix + "env_parser_mode") == "true") ? true : false;
}

extern const bool kIsFirstBuild = (getEnv("env_is_first_build") == "true") ? true : false;

// path variables
extern const std::filesystem::path kBaseSharedDir     = std::filesystem::path("/build_shared");
extern const std::filesystem::path kStockKickstartDir = kBaseSharedDir / "fedora-kickstarts";

extern const std::filesystem::path kBaseWorkingDir = std::filesystem::current_path();
extern const std::filesystem::path kBuildDir       = kBaseWorkingDir / "build";

// extern const std::filesystem::path kRepoDir = kBuildDir / "repos";
extern const std::filesystem::path kRepoDir         = "/etc/yum.repos.d";  // TODO(kd): remove after
extern const std::filesystem::path kPackageListPath = kBuildDir / "package_list.txt";
extern const std::filesystem::path kGroupListPath   = kBuildDir / "group_list.txt";

// out
extern const std::filesystem::path kBaseOutDir = kBaseWorkingDir / "out";
extern const std::filesystem::path kMainKickstartPath =
    kBaseOutDir / (std::string{std::getenv("env_os_name")} + ".ks");
extern const std::filesystem::path kFirstLoginScriptPath = kBaseOutDir / "first_login.sh";
extern const std::filesystem::path kPostBuildScriptPath  = kBaseOutDir / "post_build.sh";
extern const std::filesystem::path kPostBuildScriptNoRootPath =
    kBaseOutDir / "post_build_no_root.sh";

// log dir
extern const std::filesystem::path kKickstartLogDir = "/root" / kBaseOutDir / "log";
extern const std::filesystem::path kLogDir          = kBaseOutDir / "log";

extern const std::filesystem::path kBaseUserFileDir      = kBuildDir / "user_supplied";
extern const std::filesystem::path kConfigBuilderEnvVar  = kBuildDir / "config_builder_env_var.sh";
extern const std::filesystem::path kPrevEnvVarPath       = kBuildDir / "prev_env_var.sh";
extern const std::filesystem::path kPrevParentConfigPath = kBuildDir / "prev_config.toml";
extern const std::filesystem::path kParentConfigPath =
    kBaseUserFileDir / std::string{std::getenv("env_parent_config")};

extern const std::filesystem::path kUserFileDest = "/mnt/sysimage/usr/share/hatter_user_file";
}  // namespace build_variable
}  // namespace hatter