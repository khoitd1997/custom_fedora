#pragma once

#include <filesystem>
#include <string>

namespace hatter {
namespace build_variable {
struct CLIBuildVariable {
    std::string osName;
    int         releasever;
    std::string arch;

    bool parserMode;

    explicit CLIBuildVariable(const std::string& prefix = "");
};

extern const bool kIsFirstBuild;

// path variables
extern const std::filesystem::path kShareDir;
extern const std::filesystem::path kStockKickstartDir;
extern const std::filesystem::path kValidKeyboardPath;
extern const std::filesystem::path kValidLanguagePath;
extern const std::filesystem::path kValidTimezonePath;

extern const std::filesystem::path kBaseDir;
extern const std::filesystem::path kBuildDir;

extern const std::filesystem::path kRepoDir;
extern const std::filesystem::path kPackageListPath;
extern const std::filesystem::path kGroupListPath;

// out
extern const std::filesystem::path kBaseOutDir;
extern const std::filesystem::path kConfigBuilderEnvVar;
extern const std::filesystem::path kMainKickstartPath;
extern const std::filesystem::path kFirstLoginScriptPath;
extern const std::filesystem::path kPostBuildScriptPath;
extern const std::filesystem::path kPostBuildScriptNoRootPath;

// log dir
extern const std::filesystem::path kKickstartLogDir;
extern const std::filesystem::path kLogDir;

extern const std::filesystem::path kUserSuppliedDir;
extern const std::filesystem::path kPrevParentConfigPath;
extern const std::filesystem::path kParentConfigPath;

extern const std::filesystem::path kUserFileDest;
}  // namespace build_variable
}  // namespace hatter