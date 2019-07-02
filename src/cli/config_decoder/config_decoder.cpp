#include "config_decoder.hpp"

#include <functional>

#include "utils.hpp"

namespace hatter {
namespace config_decoder {
namespace {
template <typename T>
std::string toStr(const T &val) {
    std::stringstream ss;
    ss << val;
    return ss.str();
}
template <typename T>
std::string toStr(const std::vector<T> &vals) {
    return "[" + strJoin(vals, std::function<std::string(const T &)>{[](const T &val) {
                             return toStr<T>(val);
                         }}) +
           "]";
}
template <>
std::string toStr<std::string>(const std::string &val) {
    return "\"" + val + "\"";
}
template <>
std::string toStr<bool>(const bool &val) {
    return (val ? "true" : "false");
}
template <>
std::string toStr<std::filesystem::path>(const std::filesystem::path &val) {
    return toStr(val.string());
}

std::string getHeader(const ConfigSectionBase &conf,
                      const std::string &      topSectionName = "",
                      const bool               isListMember   = false) {
    std::string openBracket;
    std::string closeBracket;
    if (isListMember) {
        openBracket  = "[[";
        closeBracket = "]]";
    } else {
        openBracket  = "[";
        closeBracket = "]";
    }

    return openBracket + (topSectionName.empty() ? "" : topSectionName + ".") + conf.keyName +
           closeBracket;
}

template <typename T>
std::string decodeMember(const ConfigMember<T> &configMember) {
    if (configMember.value != T{}) {
        std::stringstream ss;
        ss << configMember.keyName << "=" << toStr(configMember.value);
        return ss.str();
    }
    return "";
}

std::string decode(const DistroInfo &distroInfo) {
    std::string ret;

    strAddNonEmptyLine(ret,
                       {getHeader(distroInfo),
                        decodeMember(distroInfo.kickstartTag),
                        decodeMember(distroInfo.baseSpin)});

    return ret;
}
std::string decode(const ImageInfo &imageInfo) {
    std::string ret;

    strAddNonEmptyLine(ret,
                       {getHeader(imageInfo),
                        decodeMember(imageInfo.partitionSize),
                        decodeMember(imageInfo.firstLoginScripts),
                        decodeMember(imageInfo.postBuildScripts),
                        decodeMember(imageInfo.postBuildNoRootScripts),
                        decodeMember(imageInfo.userFiles)});

    return ret;
}
std::string decode(const BuildProcessConfig &buildConfig) {
    std::string ret;

    strAddNonEmptyLine(ret,
                       {getHeader(buildConfig),
                        decodeMember(buildConfig.mockScriptPaths),
                        decodeMember(buildConfig.enableCustomCache)});

    return ret;
}
std::string decode(const CustomRepo &customRepo, const std::string &topSectionName) {
    std::string ret;

    strAddNonEmptyLine(ret,
                       {getHeader(customRepo, topSectionName, true),
                        decodeMember(customRepo.name),
                        decodeMember(customRepo.displayName),
                        decodeMember(customRepo.metaLink),
                        decodeMember(customRepo.baseurl),
                        decodeMember(customRepo.gpgcheck),
                        decodeMember(customRepo.gpgkey)});

    return ret;
}
std::string decode(const RepoConfig &repoConfig) {
    std::string ret;
    auto        customRepoDecoder = std::function<std::string(const CustomRepo &)>(
        [&](const CustomRepo &customRepo) { return decode(customRepo, repoConfig.keyName); });

    strAddNonEmptyLine(ret,
                       {getHeader(repoConfig),
                        decodeMember(repoConfig.standardRepos),
                        decodeMember(repoConfig.coprRepos),
                        strJoin(repoConfig.customRepos, customRepoDecoder, "\n")});

    return ret;
}
std::string decode(const PackageSet &packageSet, const std::string &topSectionName) {
    std::string ret;

    strAddNonEmptyLine(ret,
                       {getHeader(packageSet, topSectionName),
                        decodeMember(packageSet.installList),
                        decodeMember(packageSet.removeList)});

    return ret;
}
std::string decode(const PackageConfig &packageConfig) {
    std::string ret;

    strAddNonEmptyLine(ret,
                       {getHeader(packageConfig),
                        decode(packageConfig.rpm, packageConfig.keyName),
                        decode(packageConfig.rpmGroup, packageConfig.keyName)});

    return ret;
}
std::string decode(const MiscConfig &miscConfig) {
    std::string ret;

    strAddNonEmptyLine(ret,
                       {getHeader(miscConfig),
                        decodeMember(miscConfig.language),
                        decodeMember(miscConfig.keyboard),
                        decodeMember(miscConfig.timezone)});

    return ret;
}
}  // namespace

std::string decode(const FullConfig &fullConfig) {
    std::string ret;

    strAddLine(ret,
               {decode(fullConfig.distroInfo),
                decode(fullConfig.imageInfo),
                decode(fullConfig.buildConfig),
                decode(fullConfig.repoConfig),
                decode(fullConfig.packageConfig),
                decode(fullConfig.miscConfig)});

    return ret;
}
}  // namespace config_decoder
}  // namespace hatter
