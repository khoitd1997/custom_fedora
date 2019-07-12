#include "hatter_config_type.hpp"

#include "utils.hpp"

namespace hatter {
namespace {
std::string createDNFRepoEnableCmd(const std::string &repoName) {
    return "sudo dnf config-manager --set-enabled " + repoName;
}
}  // namespace

ConfigSectionBase::~ConfigSectionBase() {}

StandardRepo::StandardRepo(const std::string &fullRepoName,
                           const std::string &baseURL,
                           const std::string &metaURL,
                           const std::string &firstBootInstallScript)
    : fullRepoName{fullRepoName},
      baseURL{baseURL},
      metaURL{metaURL},
      firstBootInstallScript{firstBootInstallScript} {}

// clang-format off
extern const std::unordered_map<std::string, StandardRepo> kStandardRepos = {
    {"google-chrome",
     StandardRepo{"google-chrome",
                  "http://dl.google.com/linux/chrome/rpm/stable/$basearch",
                  "",
                  createDNFRepoEnableCmd("google-chrome")}},

    {"nvidia",
     StandardRepo{"rpmfusion-nonfree-nvidia-driver",
                  "",
                  "https://mirrors.rpmfusion.org/"
                  "metalink?repo=nonfree-fedora-nvidia-driver-$releasever&arch=$basearch",
                  createDNFRepoEnableCmd("rpmfusion-nonfree-nvidia-driver")}},

    {"vscode", 
    StandardRepo{"code",
                 "https://packages.microsoft.com/yumrepos/vscode",
                 "",
R""""(
sudo rpm --import https://packages.microsoft.com/keys/microsoft.asc
sudo sh -c 'echo -e \"[code]\\nname=Visual Studio Code\\nbaseurl=https://packages.microsoft.com/yumrepos/vscode\\nenabled=1\\ngpgcheck=1\\ngpgkey=https://packages.microsoft.com/keys/microsoft.asc\" > /etc/yum.repos.d/vscode.repo'
dnf check-update
sudo dnf install code -y)""""}}
};

// clang-format on
}  // namespace hatter