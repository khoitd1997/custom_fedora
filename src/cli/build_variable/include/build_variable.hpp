#pragma once

#include <filesystem>
#include <string>

namespace hatter {
namespace build_variable {
extern const std::string kOSName;
extern const std::string kInputConfig;

extern const int         kReleasever;
extern const std::string kArch;

extern const bool kClearCache;
extern const bool kParserMode;

extern const std::filesystem::path kBaseSharedDir;
extern const std::filesystem::path kFedoraKickstartDir;

extern const std::filesystem::path kBaseWorkingDir;
extern const std::filesystem::path kBuildDir;

extern const std::filesystem::path kRepoDir;
extern const std::filesystem::path kPackageListPath;
extern const std::filesystem::path kGroupListPath;

extern const std::filesystem::path kBaseOutDir;

extern const std::filesystem::path kBaseUserFileDir;
extern const std::filesystem::path kPrevParentConfigPath;
extern const std::filesystem::path kParentConfigPath;
}  // namespace build_variable
}  // namespace hatter