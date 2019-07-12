#include "build_variable.hpp"

#include <cstdlib>
#include <exception>
#include <filesystem>

namespace hatter {
namespace build_variable {
namespace {
std::string getStrEnv(const std::string& varName) {
    auto res = std::getenv(varName.c_str());
    if (res) { return std::string{res}; }
    throw std::runtime_error("failed to get environment variable " + varName);
}
bool getBoolEnv(const std::string& varName) {
    const auto str = getStrEnv(varName);
    if (str == "true") {
        return true;
    } else if (str == "false") {
        return false;
    }
    throw std::runtime_error("bool variable " + varName + " has invalid value");
}
int getIntEnv(const std::string& varName) {
    const auto str = getStrEnv(varName);
    return std::stoi(str);
}
std::filesystem::path getPathEnv(const std::string& varName) {
    const auto str = getStrEnv(varName);
    return std::filesystem::path{str};
}
}  // namespace

CLIBuildVariable::CLIBuildVariable(const std::string& prefix) {
    const std::string fullPrefix = prefix.empty() ? "" : prefix + "_";

    osName     = getStrEnv(fullPrefix + "env_os_name");
    releasever = getIntEnv(fullPrefix + "env_releasever");
    arch       = getStrEnv(fullPrefix + "env_arch");
    parserMode = getBoolEnv(fullPrefix + "env_parser_mode");
}

// path variables
extern const std::filesystem::path kBaseDir  = getPathEnv("env_base_dir");
extern const std::filesystem::path kBuildDir = getPathEnv("env_build_dir");

extern const std::filesystem::path kShareDir          = getPathEnv("env_base_dir");
extern const std::filesystem::path kStockKickstartDir = getPathEnv("env_stock_kickstart_dir");
extern const std::filesystem::path kValidKeyboardPath = getPathEnv("env_valid_keyboard_path");
extern const std::filesystem::path kValidLanguagePath = getPathEnv("env_valid_language_path");
extern const std::filesystem::path kValidTimezonePath = getPathEnv("env_valid_timezone_path");

extern const std::filesystem::path kRepoDir         = getPathEnv("env_repo_dir");
extern const std::filesystem::path kRepoConfPath    = getPathEnv("env_repo_conf");
extern const std::filesystem::path kRepoLogPath     = getPathEnv("env_repo_log");
extern const std::filesystem::path kPackageListPath = getPathEnv("env_package_list_path");
extern const std::filesystem::path kGroupListPath   = getPathEnv("env_group_list_path");

// out
extern const std::filesystem::path kBaseOutDir        = getPathEnv("env_out_dir");
extern const std::filesystem::path kMainKickstartPath = getPathEnv("env_main_kickstart_path");
extern const std::filesystem::path kFirstLoginScriptPath =
    getPathEnv("env_first_login_script_path");
extern const std::filesystem::path kPostBuildScriptPath = getPathEnv("env_post_build_script_path");
extern const std::filesystem::path kPostBuildScriptNoRootPath =
    getPathEnv("env_post_build_script_no_root_path");

// log dir
extern const std::filesystem::path kKickstartLogDir = getPathEnv("env_kickstart_log_dir");
extern const std::filesystem::path kLogDir          = getPathEnv("env_log_dir");

extern const std::filesystem::path kUserSuppliedDir = getPathEnv("env_user_supplied_dir");
extern const std::filesystem::path kConfigBuilderEnvVar =
    getPathEnv("env_config_builder_env_var_path");
extern const std::filesystem::path kPrevEnvVarPath = getPathEnv("env_prev_var_path");
extern const std::filesystem::path kPrevParentConfigPath =
    getPathEnv("env_prev_parent_config_path");
extern const std::filesystem::path kParentConfigPath = getPathEnv("env_parent_config_path");

extern const std::filesystem::path kUserFileDest = getPathEnv("env_user_file_dest");

extern const bool kIsFirstBuild = !(std::filesystem::exists(kPrevEnvVarPath));
}  // namespace build_variable
}  // namespace hatter