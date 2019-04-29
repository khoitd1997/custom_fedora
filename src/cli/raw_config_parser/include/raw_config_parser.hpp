#ifndef _RAW_CONFIG_PARSER_HPP
#define _RAW_CONFIG_PARSER_HPP
#include <string>
#include <vector>

#include "toml11/toml.hpp"
namespace hatter {
struct BaseConfig {
    bool              isValid = true;
    const std::string sectionName;

    void printSection(const std::string& colorCode);
    BaseConfig(const std::string& sectionName);
    virtual ~BaseConfig() = 0;
};

struct BasicConfig : public BaseConfig {
    std::string imageVersion;
    std::string imageArch;
    std::string kickstartTag;
    std::string baseSpin;

    std::string firstLoginScript;
    std::string postBuildScript;
    std::string postBuildNoRootScript;

    std::vector<std::string> userFiles;

    explicit BasicConfig(const toml::table& rawConfig);
};

struct Repo {
    std::string name;
    std::string displayName;

    std::string metaLink;
    std::string baseurl;

    bool        gpgcheck;
    std::string gpgkey;

    void from_toml(const toml::value& v);
};

struct RepoConfig : public BaseConfig {
    std::vector<std::string> standardRepos;
    std::vector<std::string> coprRepos;
    std::vector<Repo>        customRepos;

    explicit RepoConfig(const toml::table& rawConfig);
};

struct PackageSet {
    std::vector<std::string> installList;
    std::vector<std::string> removeList;

    bool parse(const toml::table& rawPackageConfig, const std::string& tableName);
};

struct PackageConfig : public BaseConfig {
    PackageSet rpm;
    PackageSet rpmGroup;

    explicit PackageConfig(const toml::table& rawConfig);
};
}  // namespace hatter
#endif