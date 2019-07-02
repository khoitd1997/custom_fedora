#include "config_builder.hpp"

#include <algorithm>
#include <regex>

#include "logger.hpp"
#include "utils.hpp"

namespace hatter {
namespace config_builder {
namespace {
std::string buildKickstartRepo(const std::string& repoName,
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

std::string buildCommandWithListParam(const std::string&              cmd,
                                      const std::vector<std::string>& params,
                                      const std::string&              endLine,
                                      const std::string&              beginComment = "") {
    std::string ret;
    std::string align(cmd.length(), ' ');

    if (!beginComment.empty() && !params.empty()) { strAddLine(ret, "# " + beginComment); }
    for (auto param = params.cbegin(); param != params.cend(); ++param) {
        std::string tempLine;
        logger::info("adding param for " + beginComment);
        if (param == params.cbegin()) {
            tempLine += cmd + " " + *param;
        } else {
            tempLine += align + " " + *param;
        }

        if (param == params.cend() - 1) {
            tempLine += " " + endLine;
        } else {
            tempLine += " \\";
        }
        strAddLine(ret, tempLine);
    }

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
std::string joinFile(const std::vector<std::filesystem::path>& files) {
    std::string ret;

    for (const auto& file : files) { strAddNonEmptyLine(ret, readFile(file)); }

    return ret;
}
std::string buildKickstartSection(const std::string& header, const std::string& content) {
    if (!content.empty()) { return "\n\%" + header + "\n" + content + "\n\%end\n"; }
    return "";
}
}  // namespace

std::string buildBaseKickstartGitTag(const int releasever, const std::string& targetTag) {
    std::string       finalTag;
    const std::string releaseverStr = std::to_string(releasever);
    const std::string baseGitCmd = "git -C " + hatter::build_variable::kStockKickstartDir.string();

    if (targetTag == "latest") {
        std::string temp;
        hatter::execCommand(
            "curl -sS https://pagure.io/api/0/fedora-kickstarts/git/tags | jq -r '.tags | .[]' ",
            temp);
        const auto allTags = hatter::strSplit(temp, "\n");

        int        maxTag = -1;
        const auto regex  = std::regex("0." + releaseverStr + ".([0-9]+)");
        for (const auto& tag : allTags) {
            std::smatch match;
            if (std::regex_match(tag, match, regex)) {
                maxTag = std::max(maxTag, std::stoi(match[0].str()));
            }
        }

        if (maxTag < 0) {
            throw std::runtime_error("can't find latest git tag for releasever " + releaseverStr);
        }
        finalTag = "0." + releaseverStr + "." + std::to_string(maxTag);
    } else {
        finalTag = targetTag;
    }

    return finalTag;
}
std::string buildBaseKickstartName(const std::string& baseSpin) {
    return "fedora-live-" + baseSpin;
}

std::string buildUserFileTransferCommand(const std::vector<std::filesystem::path> userFiles) {
    std::string ret;

    std::string userFileStr;
    for (const auto& userFile : userFiles) { userFileStr += userFile.string() + " "; }

    strAddLine(ret,
               {"cp -r " + userFileStr + " " + build_variable::kUserFileDest.string(),
                "chmod -R a+r+x " + build_variable::kUserFileDest.string()});

    return ret;
}
std::pair<std::string, std::string> buildRepoList(const RepoConfig& repoConfig) {
    std::string ksStr;
    std::string firstBootStr;

    if (!repoConfig.standardRepos.value.empty()) {
        strAddLine(firstBootStr, "sudo dnf install fedora-workstation-repositories -y");
    }
    for (const auto& stdRepo : repoConfig.standardRepos.value) {
        std::string ksRepoName;
        std::string ksBaseURL;
        std::string ksMetaLink;

        if (stdRepo == "google-chrome") {
            ksRepoName = "google-chrome";
            ksBaseURL  = "http://dl.google.com/linux/chrome/rpm/stable/$basearch";

            strAddLine(firstBootStr, {"sudo dnf config-manager --set-enabled " + ksRepoName});
        } else if (stdRepo == "nvidia") {
            ksRepoName = "rpmfusion-nonfree-nvidia-driver";
            ksMetaLink =
                "https://mirrors.rpmfusion.org/"
                "metalink?repo=nonfree-fedora-nvidia-driver-$releasever&arch=$basearch";

            strAddLine(firstBootStr, {"sudo dnf config-manager --set-enabled " + ksRepoName});
        } else if (stdRepo == "vscode") {
            // source: https://code.visualstudio.com/docs/setup/linux
            ksRepoName = "code";
            ksBaseURL  = "https://packages.microsoft.com/yumrepos/vscode";

            strAddLine(
                firstBootStr,
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
        strAddLine(ksStr, {buildKickstartRepo(ksRepoName, ksBaseURL, ksMetaLink)});
    }

    strAddNonEmptyLine(firstBootStr,
                       buildCommandWithListParam(
                           "sudo dnf copr enable", repoConfig.coprRepos.value, "-y", "copr repos"));
    for (const auto& coprRepo : repoConfig.coprRepos.value) {
        strAddLine(ksStr, {buildKickstartRepo(coprRepo, getCOPRBaseURL(coprRepo), "")});
    }

    // TODO(kd): Recheck custom repo handling
    std::vector<std::string> customRepoNames;
    for (const auto& customRepo : repoConfig.customRepos) {
        strAddLine(
            ksStr,
            {buildKickstartRepo(
                customRepo.name.value, customRepo.baseurl.value, customRepo.metaLink.value)});
        customRepoNames.push_back(customRepo.name.value);
    }
    strAddNonEmptyLine(
        firstBootStr,
        buildCommandWithListParam(
            "sudo dnf config-manager --set-enabled", customRepoNames, "", "custom repos"));

    return std::make_pair(ksStr, firstBootStr);
}
std::pair<std::string, std::string> buildPackageCommand(const PackageConfig& currPkgConfig,
                                                        const PackageConfig& prevPkgConfig,
                                                        const bool           isFullBuild) {
    std::string install;
    std::string remove;

    strAddNonEmptyLine(
        remove,
        {buildCommandWithListParam(
             "dnf remove", currPkgConfig.rpm.removeList.value, "-y", "remove rpm package"),
         buildCommandWithListParam("dnf group remove",
                                   currPkgConfig.rpmGroup.removeList.value,
                                   "-y",
                                   "remove rpm group")});
    if (isFullBuild) {
        strAddLine(install, "# rpm package");
        strAddLine(install, currPkgConfig.rpm.installList.value);
        strAddLine(install, {"", "# rpm group"});
        for (const auto& groupInstall : currPkgConfig.rpmGroup.installList.value) {
            strAddLine(install, "@" + groupInstall);
        }
    } else {
        strAddNonEmptyLine(
            remove,
            {buildCommandWithListParam("dnf remove",
                                       subtractVector(prevPkgConfig.rpm.installList.value,
                                                      currPkgConfig.rpm.installList.value),
                                       "-y",
                                       "diff remove rpm package"),
             buildCommandWithListParam("dnf group remove",
                                       subtractVector(prevPkgConfig.rpmGroup.installList.value,
                                                      currPkgConfig.rpmGroup.installList.value),
                                       "-y",
                                       "diff remove rpm group")});
    }

    return std::make_pair(install, remove);
}
std::string buildMisc(const MiscConfig& miscConfig) {
    std::string ret;

    strAddLine(ret,
               {"lang " + miscConfig.language.value,
                "keyboard " + miscConfig.keyboard.value,
                "timezone " + miscConfig.timezone.value});

    return ret;
}

bool needRebuild(const FullConfig&                       currConfig,
                 const FullConfig&                       prevConfig,
                 const build_variable::CLIBuildVariable& currBuildVar,
                 const build_variable::CLIBuildVariable& prevBuildVar) {
    if ((currBuildVar.arch != prevBuildVar.arch) ||
        (currBuildVar.releasever != prevBuildVar.releasever) ||
        (currConfig.distroInfo != prevConfig.distroInfo)) {
        return true;
    }
    return false;
}

void buildBaseLayer(const FullConfig&                       currConfig,
                    const build_variable::CLIBuildVariable& currBuildVar) {
    std::string configBuilderEnvFile;

    strAddLine(configBuilderEnvFile,
               {"export env_base_kickstart_name=" +
                    buildBaseKickstartName(currConfig.distroInfo.baseSpin.value),
                "export env_base_kickstart_tag=" +
                    buildBaseKickstartGitTag(currBuildVar.releasever,
                                             currConfig.distroInfo.kickstartTag.value)});

    writeFile(configBuilderEnvFile, build_variable::kConfigBuilderEnvVar);
}
void buildVolatileLayer(const FullConfig& currConfig,
                        const FullConfig& prevConfig,
                        const bool        isFullBuild) {
    std::string kickstartFile;

    std::string firstLoginScript;
    std::string postBuildScript;
    std::string postBuildNoRootScript;

    strAddNonEmptyLine(kickstartFile, buildMisc(currConfig.miscConfig));

    const auto packageCmd =
        buildPackageCommand(currConfig.packageConfig, prevConfig.packageConfig, isFullBuild);
    strAddNonEmptyLine(kickstartFile, buildKickstartSection("packages", packageCmd.first));
    strAddNonEmptyLine(postBuildScript, packageCmd.second);

    const auto repoList = buildRepoList(currConfig.repoConfig);
    strAddNonEmptyLine(kickstartFile, repoList.first);
    strAddNonEmptyLine(firstLoginScript, repoList.second);

    strAddNonEmptyLine(firstLoginScript, joinFile(currConfig.imageInfo.firstLoginScripts.value));
    strAddNonEmptyLine(postBuildScript, joinFile(currConfig.imageInfo.postBuildScripts.value));

    strAddNonEmptyLine(postBuildNoRootScript,
                       {buildUserFileTransferCommand(currConfig.imageInfo.userFiles.value),
                        joinFile(currConfig.imageInfo.postBuildNoRootScripts.value)});

    strAddNonEmptyLine(
        kickstartFile,
        {buildKickstartSection(
             "post --erroronfail --log=" + build_variable::kKickstartLogDir.string(),
             postBuildScript),
         buildKickstartSection("post --nochroot --log=" + build_variable::kKickstartLogDir.string(),
                               postBuildNoRootScript)});
    writeFile(kickstartFile, build_variable::kMainKickstartPath);

    writeFile(firstLoginScript, build_variable::kFirstLoginScriptPath);
    writeFile(postBuildScript, build_variable::kPostBuildScriptPath);
    writeFile(postBuildNoRootScript, build_variable::kPostBuildScriptNoRootPath);
}

void build(const FullConfig&                       currConfig,
           const FullConfig&                       prevConfig,
           const build_variable::CLIBuildVariable& currBuildVar,
           const build_variable::CLIBuildVariable& prevBuildVar) {
    const auto isFullBuild = build_variable::kIsFirstBuild ||
                             needRebuild(currConfig, prevConfig, currBuildVar, prevBuildVar);

    if (isFullBuild) { buildBaseLayer(currConfig, currBuildVar); }
    buildVolatileLayer(currConfig, prevConfig, isFullBuild);
}

}  // namespace config_builder
}  // namespace hatter