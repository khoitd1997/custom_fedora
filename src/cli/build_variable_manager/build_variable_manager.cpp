#include "build_variable_manager.hpp"

#include <cstdlib>
#include <filesystem>

namespace hatter {
namespace build_variable_manager {
extern const std::string kOSName      = std::string{std::getenv("env_os_name")};
extern const std::string kInputConfig = std::string{std::getenv("env_input_config")};

extern const int         kReleasever = std::stoi(std::string{std::getenv("env_releasever")});
extern const std::string kArch       = std::string{std::getenv("env_arch")};

extern const bool kClearCache =
    (std::string{std::getenv("env_clear_cache")} == "true") ? true : false;
}  // namespace build_variable_manager
}  // namespace hatter