#ifndef _RAW_CONFIG_PARSER_HPP
#define _RAW_CONFIG_PARSER_HPP
#include <string>
#include <vector>

#include "toml11/toml.hpp"

#include "default_config.hpp"

namespace hatter {
struct BaseConfig {
   public:
    virtual ~BaseConfig() = 0;

    explicit operator bool() const { return isValid_; }
    bool     isPresent() const { return isPresent_; }

   protected:
    bool isValid_   = true;
    bool isPresent_ = false;

    toml::table getBaseTable_(const toml::table& rawConfig,
                              const std::string& tableName,
                              const std::string& colorCode = "");
};

struct DistroInfo : public BaseConfig {
    std::string imageVersion;
    std::string imageArch    = kDefaultImageArch;
    std::string kickstartTag = kDefaultKickstartTag;
    std::string baseSpin;
    std::string osName;

    explicit DistroInfo(const toml::table& rawConfig);
};

struct ImageInfo : public BaseConfig {
    int                      partitionSize;
    std::string              firstLoginScript;
    std::string              postBuildScript;
    std::string              postBuildNoRootScript;
    std::vector<std::string> userFiles;

    explicit ImageInfo(const toml::table& rawConfig);
};

struct BuildProcessConfig : public BaseConfig {
    bool enableCustomCache = kDefaultEnableCustomCache;

    explicit BuildProcessConfig(const toml::table& rawConfig);
};

struct Repo : public BaseConfig {
    std::string name;
    std::string displayName;

    std::string metaLink;
    std::string baseurl;

    bool        gpgcheck = false;
    std::string gpgkey;

    void from_toml(const toml::value& v);
};

struct RepoConfig : public BaseConfig {
    std::vector<std::string> standardRepos;
    std::vector<std::string> coprRepos;
    std::vector<Repo>        customRepos;

    explicit RepoConfig(const toml::table& rawConfig);
};

struct PackageSet : public BaseConfig {
    std::vector<std::string> installList;
    std::vector<std::string> removeList;

    PackageSet();
    PackageSet(const toml::table& rawPackageConfig, const std::string& tableName);
};

struct PackageConfig : public BaseConfig {
    PackageSet rpm;
    PackageSet rpmGroup;

    explicit PackageConfig(const toml::table& rawConfig);
};

struct MiscConfig : public BaseConfig {
    std::string language = kDefaultLanguage;
    std::string keyboard;
    std::string timezone;

    explicit MiscConfig(const toml::table& rawConfig);
};
}  // namespace hatter
#endif