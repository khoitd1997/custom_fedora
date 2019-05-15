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

// struct ImageInfo  {
//     int                      partitionSize;
//     std::string              firstLoginScript;
//     std::string              postBuildScript;
//     std::string              postBuildNoRootScript;
//     std::vector<std::string> userFiles;

//     bool merge(const ImageInfo& target);
//     explicit ImageInfo(const RawTOMLConfig& rawConfig);
// };

// struct BuildProcessConfig  {
//     bool        enableCustomCache = kDefaultEnableCustomCache;
//     std::string mockScript;

//     explicit BuildProcessConfig(const RawTOMLConfig& rawConfig);
// };

struct Repo {
    std::string name;
    std::string displayName;

    std::string metaLink;
    std::string baseurl;

    bool        gpgcheck = false;
    std::string gpgkey;

    bool operator==(const Repo& r) const {
        return (this->name == r.name) && (this->displayName == r.displayName) &&
               (this->metaLink == r.metaLink) && (this->baseurl == r.baseurl) &&
               (this->gpgcheck == r.gpgcheck) && (this->gpgkey == r.gpgkey);
    }
    bool operator!=(const Repo& r) const { return !(this->operator==(r)); }
};

struct RepoConfig {
    std::vector<std::string> standardRepos;
    std::vector<std::string> coprRepos;
    std::vector<Repo>        customRepos;
};

struct PackageSet {
    std::vector<std::string> installList;
    std::vector<std::string> removeList;
};

struct PackageConfig {
    PackageSet rpm;
    PackageSet rpmGroup;
};

// struct MiscConfig  {
//     std::string language = kDefaultLanguage;
//     std::string keyboard;
//     std::string timezone;

//     explicit MiscConfig(const RawTOMLConfig& rawConfig);
// };

struct FullConfig {
    RepoConfig repoConfig;
};

}  // namespace hatter

namespace std {
template <>
struct hash<hatter::Repo> {
    size_t operator()(const hatter::Repo& r) const { return hash<std::string>()(r.name); }
};
}  // namespace std