#pragma once

#include <filesystem>
#include <string>
#include <vector>

#include "toml11/toml.hpp"

#include "default_config.hpp"

namespace hatter {
// struct RawTOMLConfig {
//     toml::table config;
//     explicit    operator bool() const { return isValid_; }

//     explicit RawTOMLConfig(const std::string& filePath);

//    private:
//     bool isValid_ = false;
// };

// struct BaseConfig {
//    public:
//     virtual ~BaseConfig() = 0;

//     explicit operator bool() const { return isValid_; }
//     bool     isPresent() const { return isPresent_; }

//    protected:
//     bool isValid_   = true;
//     bool isPresent_ = false;

//     toml::table getBaseTable_(const RawTOMLConfig& rawConfig,
//                               const std::string&   tableName,
//                               const std::string&   colorCode = "");
//     toml::table getBaseTable_(const toml::table& rawConfig,
//                               const std::string& tableName,
//                               const std::string& colorCode = "");
// };

// struct DistroInfo  {
//     std::string kickstartTag = kDefaultKickstartTag;
//     std::string baseSpin;
//     std::string osName;

//     bool merge(const DistroInfo& target);

//     explicit DistroInfo(const RawTOMLConfig& rawConfig);
// };

struct ImageInfo {
    int                                partitionSize;
    std::vector<std::filesystem::path> firstLoginScripts;
    std::vector<std::filesystem::path> postBuildScripts;
    std::vector<std::filesystem::path> postBuildNoRootScripts;
    std::vector<std::filesystem::path> userFiles;
};

struct BuildProcessConfig {
    std::vector<std::filesystem::path> mockScriptPaths;
    bool                               enableCustomCache = kDefaultEnableCustomCache;
};

struct CustomRepo {
    std::string name;
    std::string displayName;

    std::string metaLink;
    std::string baseurl;

    bool        gpgcheck = false;
    std::string gpgkey;

    bool operator==(const CustomRepo& r) const {
        return (this->name == r.name) && (this->displayName == r.displayName) &&
               (this->metaLink == r.metaLink) && (this->baseurl == r.baseurl) &&
               (this->gpgcheck == r.gpgcheck) && (this->gpgkey == r.gpgkey);
    }
    bool operator!=(const CustomRepo& r) const { return !(this->operator==(r)); }
};

struct RepoConfig {
    std::vector<std::string> standardRepos;
    std::vector<std::string> coprRepos;
    std::vector<CustomRepo>  customRepos;
};

struct PackageSet {
    enum class PackageType { rpm, rpm_group };

    PackageType              packageType;
    std::vector<std::string> installList;
    std::vector<std::string> removeList;

    explicit PackageSet(const PackageType pkgType) : packageType(pkgType) {}
};

struct PackageConfig {
    PackageSet rpm{PackageSet::PackageType::rpm};
    PackageSet rpmGroup{PackageSet::PackageType::rpm_group};
};

struct MiscConfig {
    std::string language = kDefaultLanguage;
    std::string keyboard;
    std::string timezone;
};

struct FullConfig {
    ImageInfo          imageInfo;
    BuildProcessConfig buildConfig;
    RepoConfig         repoConfig;
    PackageConfig      packageConfig;
    MiscConfig         miscConfig;
};
}  // namespace hatter

namespace std {
template <>
struct hash<hatter::CustomRepo> {
    size_t operator()(const hatter::CustomRepo& r) const { return hash<std::string>()(r.name); }
};
}  // namespace std