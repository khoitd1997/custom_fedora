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

template <typename T>
struct ConfigMember {
    const std::string keyName;
    T                 value;
    bool              isOptional;

    ConfigMember(const std::string &keyName) : ConfigMember{keyName, {}, true} {}
    ConfigMember(const std::string &keyName, const T &defaultValue)
        : ConfigMember{keyName, defaultValue, true} {}
    ConfigMember(const std::string &keyName, const T &defaultValue, const bool isOptional)
        : keyName{keyName}, value{defaultValue}, isOptional{isOptional} {}
};

struct ImageInfo {
    ConfigMember<int>                                partitionSize{"partition_size"};
    ConfigMember<std::vector<std::filesystem::path>> firstLoginScripts{"first_login_script"};
    ConfigMember<std::vector<std::filesystem::path>> postBuildScripts{"post_build_scripts"};
    ConfigMember<std::vector<std::filesystem::path>> postBuildNoRootScripts{
        "post_build_script_no_chroots"};
    ConfigMember<std::vector<std::filesystem::path>> userFiles{"user_files"};
};

struct BuildProcessConfig {
    ConfigMember<std::vector<std::filesystem::path>> mockScriptPaths{"custom_mock_script"};
    ConfigMember<bool> enableCustomCache{"enable_custom_cache", kDefaultEnableCustomCache};
};

// TODO: move this inside Repo
struct CustomRepo {
    ConfigMember<std::string> name{"name"};
    ConfigMember<std::string> displayName{"display_name"};

    ConfigMember<std::string> metaLink{"metalink"};
    ConfigMember<std::string> baseurl{"baseurl"};

    ConfigMember<bool>        gpgcheck{"gpgcheck", false};
    ConfigMember<std::string> gpgkey{"gpgkey"};

    bool operator==(const CustomRepo &r) const {
        return (this->name.value == r.name.value) &&
               (this->displayName.value == r.displayName.value) &&
               (this->metaLink.value == r.metaLink.value) &&
               (this->baseurl.value == r.baseurl.value) &&
               (this->gpgcheck.value == r.gpgcheck.value) && (this->gpgkey.value == r.gpgkey.value);
    }
    bool operator!=(const CustomRepo &r) const { return !(this->operator==(r)); }
};

struct RepoConfig {
    ConfigMember<std::vector<std::string>> standardRepos{"standard_repos"};
    ConfigMember<std::vector<std::string>> coprRepos{"copr_repos"};
    ConfigMember<std::vector<CustomRepo>>  customRepos{"custom_repos"};
};

struct PackageSet {
    enum class PackageType { rpm, rpm_group };

    PackageType packageType;

    ConfigMember<std::vector<std::string>> installList{"install"};
    ConfigMember<std::vector<std::string>> removeList{"remove"};

    explicit PackageSet(const PackageType pkgType) : packageType(pkgType) {}
};

struct PackageConfig {
    ConfigMember<PackageSet> rpm{"rpm", PackageSet{PackageSet::PackageType::rpm}};
    ConfigMember<PackageSet> rpmGroup{"rpm_group", PackageSet{PackageSet::PackageType::rpm_group}};
};

struct MiscConfig {
    ConfigMember<std::string> language{"language", kDefaultLanguage};
    ConfigMember<std::string> keyboard{"keyboard"};
    ConfigMember<std::string> timezone{"timezone"};
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
    size_t operator()(const hatter::CustomRepo &r) const {
        return hash<std::string>()(r.name.value);
    }
};
}  // namespace std