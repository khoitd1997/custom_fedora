#pragma once

#include <filesystem>
#include <string>
#include <vector>

#include "magic_enum.hpp"
#include "toml11/toml.hpp"

#include "default_config.hpp"

namespace hatter {
struct ConfigSectionBase {
    std::string keyName;

    ConfigSectionBase(const std::string &keyName) : keyName{keyName} {}
    virtual ~ConfigSectionBase() = 0;
};

template <typename T>
struct ConfigMember {
    std::string keyName;
    T           value      = {};
    bool        isOptional = true;
};

struct DistroInfo : public ConfigSectionBase {
    ConfigMember<std::string> kickstartTag{"base_kickstart_tag", .value = kDefaultKickstartTag};
    ConfigMember<std::string> baseSpin{"base_spin"};

    DistroInfo() : ConfigSectionBase{"distro_info"} {}
};

struct ImageInfo : public ConfigSectionBase {
    ConfigMember<int>                                partitionSize{"partition_size"};
    ConfigMember<std::vector<std::filesystem::path>> firstLoginScripts{"first_login_script"};
    ConfigMember<std::vector<std::filesystem::path>> postBuildScripts{"post_build_scripts"};
    ConfigMember<std::vector<std::filesystem::path>> postBuildNoRootScripts{
        "post_build_script_no_chroots"};
    ConfigMember<std::vector<std::filesystem::path>> userFiles{"user_files"};

    ImageInfo() : ConfigSectionBase{"image_info"} {}
};

struct BuildProcessConfig : public ConfigSectionBase {
    ConfigMember<std::vector<std::filesystem::path>> mockScriptPaths{"custom_mock_script"};
    ConfigMember<bool> enableCustomCache{"enable_custom_cache", .value = kDefaultEnableCustomCache};

    BuildProcessConfig() : ConfigSectionBase{"build_process"} {}
};

// TODO: move this inside Repo
struct CustomRepo : public ConfigSectionBase {
    ConfigMember<std::string> name{"name"};
    ConfigMember<std::string> displayName{"display_name"};

    ConfigMember<std::string> metaLink{"metalink"};
    ConfigMember<std::string> baseurl{"baseurl"};

    ConfigMember<bool>        gpgcheck{"gpgcheck", .isOptional = false};
    ConfigMember<std::string> gpgkey{"gpgkey"};

    bool operator==(const CustomRepo &r) const {
        return (this->name.value == r.name.value) &&
               (this->displayName.value == r.displayName.value) &&
               (this->metaLink.value == r.metaLink.value) &&
               (this->baseurl.value == r.baseurl.value) &&
               (this->gpgcheck.value == r.gpgcheck.value) && (this->gpgkey.value == r.gpgkey.value);
    }
    bool operator!=(const CustomRepo &r) const { return !(this->operator==(r)); }

    CustomRepo() : ConfigSectionBase{"custom_repo"} {}
};

struct RepoConfig : public ConfigSectionBase {
    ConfigMember<std::vector<std::string>> standardRepos{"standard_repos"};
    ConfigMember<std::vector<std::string>> coprRepos{"copr_repos"};
    std::vector<CustomRepo>                customRepos;

    RepoConfig() : ConfigSectionBase{"repo"} {}
};

struct PackageSet : public ConfigSectionBase {
    enum class PackageType { rpm, rpm_group };

    PackageType packageType;

    ConfigMember<std::vector<std::string>> installList{"install"};
    ConfigMember<std::vector<std::string>> removeList{"remove"};

    explicit PackageSet(const PackageType pkgType)
        : ConfigSectionBase{std::string(magic_enum::enum_name(pkgType))}, packageType{pkgType} {}
};

struct PackageConfig : public ConfigSectionBase {
    PackageSet rpm{PackageSet{PackageSet::PackageType::rpm}};
    PackageSet rpmGroup{PackageSet{PackageSet::PackageType::rpm_group}};

    PackageConfig() : ConfigSectionBase{"package"} {}
};

struct MiscConfig : public ConfigSectionBase {
    ConfigMember<std::string> language{"language", .value = kDefaultLanguage};
    ConfigMember<std::string> keyboard{"keyboard"};
    ConfigMember<std::string> timezone{"timezone"};

    MiscConfig() : ConfigSectionBase{"misc"} {}
};

struct FullConfig {
    DistroInfo         distroInfo;
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