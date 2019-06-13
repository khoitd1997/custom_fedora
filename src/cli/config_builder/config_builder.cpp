#include "config_builder.hpp"

#include <algorithm>

#include "build_variable.hpp"
#include "utils.hpp"

namespace hatter {
namespace config_builder {
namespace {
std::string generateKickstartRepo(const std::string& repoName,
                                  const std::string& repoBaseURL,
                                  const std::string& repoMetaLink,
                                  const int          cost = 50) {
    std::string ret;
    ret += "repo --name=" + repoName;
    ret += repoBaseURL.empty() ? "" : " --baseurl=" + repoBaseURL;
    ret += repoMetaLink.empty() ? "" : " --metalink=" + repoMetaLink;
    ret += " --cost=" + std::to_string(cost);

    return ret;
}
std::string getCOPRBaseURL(std::string coprRepo) {
    std::string ret;
    std::replace(coprRepo.begin(), coprRepo.end(), '/', ':');
    std::string coprRepoFileName = "_copr:copr.fedorainfracloud.org:" + coprRepo + ".repo";
    std::string coprRepoFilePath = (build_variable::kRepoDir / coprRepoFileName).string();

    execCommand("rg baseurl " + coprRepoFilePath +
                    " | sed "
                    "'s/.*=//'",
                ret);

    if (ret.empty()) { throw std::runtime_error("failed to get copr baseurl for " + coprRepo); }

    return ret;
}
}  // namespace

std::string generateIncludeKickstart(const DistroInfo& distroInfo) {
    std::string ret;
    strAddLine(ret,
               {"%include " + build_variable::kFedoraKickstartDir.string() + "/fedora-live-" +
                distroInfo.baseSpin.value});
    return ret;
}

std::pair<std::string, std::string> generateRepoList(const RepoConfig& repoConfig) {
    std::string ksStr;
    std::string postStr;

    if (!repoConfig.standardRepos.value.empty()) {
        strAddLine(postStr, {"sudo dnf install fedora-workstation-repositories -y"});
    }
    for (const auto& stdRepo : repoConfig.standardRepos.value) {
        std::string ksRepoName;
        std::string ksBaseURL;
        std::string ksMetaLink;

        if (stdRepo == "google-chrome") {
            ksRepoName = "google-chrome";
            ksBaseURL  = "http://dl.google.com/linux/chrome/rpm/stable/$basearch";

            strAddLine(postStr, {"sudo dnf config-manager --set-enabled " + ksRepoName});
        } else if (stdRepo == "nvidia") {
            ksRepoName = "rpmfusion-nonfree-nvidia-driver";
            ksMetaLink =
                "https://mirrors.rpmfusion.org/"
                "metalink?repo=nonfree-fedora-nvidia-driver-$releasever&arch=$basearch";

            strAddLine(postStr, {"sudo dnf config-manager --set-enabled " + ksRepoName});
        } else if (stdRepo == "vscode") {
            // source: https://code.visualstudio.com/docs/setup/linux
            ksRepoName = "code";
            ksBaseURL  = "https://packages.microsoft.com/yumrepos/vscode";

            strAddLine(
                postStr,
                {"sudo rpm --import https://packages.microsoft.com/keys/microsoft.asc",
                 "sudo sh -c 'echo -e \"[code]\\nname=Visual Studio "
                 "Code\\nbaseurl=https://packages.microsoft.com/yumrepos/"
                 "vscode\\nenabled=1\\ngpgcheck=1\\ngpgkey=https://packages.microsoft.com/keys/"
                 "microsoft.asc\" > /etc/yum.repos.d/vscode.repo'",
                 "dnf check-update",
                 "sudo dnf install code -y"});
        } else {
            throw std::runtime_error("unknown standard repo");
        }
        strAddLine(ksStr, {generateKickstartRepo(ksRepoName, ksBaseURL, ksMetaLink)});
    }

    for (const auto& coprRepo : repoConfig.coprRepos.value) {
        strAddLine(ksStr, {generateKickstartRepo(coprRepo, getCOPRBaseURL(coprRepo), "")});
        strAddLine(postStr, {"sudo dnf copr enable " + coprRepo + " -y"});
    }

    for (const auto& customRepo : repoConfig.customRepos) {
        strAddLine(
            ksStr,
            {generateKickstartRepo(
                customRepo.name.value, customRepo.baseurl.value, customRepo.metaLink.value)});
        strAddLine(postStr, {"sudo dnf config-manager --set-enabled " + customRepo.name.value});
    }

    return std::make_pair(ksStr, postStr);
}
std::string generatePackageList(const PackageConfig& pkgConfig) {
    std::string ret;
    strAddLine(ret, {"%packages"});

    strAddLine(ret, {"# rpm package"});
    strAddLine(ret, {"# install"});
    for (const auto& installRPM : pkgConfig.rpm.installList.value) {
        strAddLine(ret, {installRPM});
    }
    strAddLine(ret, {"", "# remove"});
    for (const auto& removeRPM : pkgConfig.rpm.removeList.value) {
        strAddLine(ret, {"-" + removeRPM});
    }
    strAddLine(ret, {""});

    strAddLine(ret, {"# rpm group"});
    strAddLine(ret, {"# install"});
    for (const auto& groupInstall : pkgConfig.rpmGroup.installList.value) {
        strAddLine(ret, {"@" + groupInstall});
    }
    strAddLine(ret, {"", "# remove"});
    for (const auto& groupRemove : pkgConfig.rpmGroup.removeList.value) {
        strAddLine(ret, {"-@" + groupRemove});
    }

    strAddLine(ret, {"%end"});
    return ret;
}
std::string generateMisc(const MiscConfig& miscConfig) {
    std::string ret;

    strAddLine(ret,
               {"lang " + miscConfig.language.value,
                "keyboard " + miscConfig.keyboard.value,
                "timezone " + miscConfig.timezone.value});

    return ret;
}
}  // namespace config_builder
}  // namespace hatter