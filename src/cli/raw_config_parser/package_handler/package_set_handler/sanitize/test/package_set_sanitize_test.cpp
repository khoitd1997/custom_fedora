#include "package_set_sanitize.hpp"

#include <map>

#include "gmock/gmock.h"

#include "common_sanitize.hpp"
#include "raw_config_parser_test_utils.hpp"
#include "toml11/toml.hpp"
#include "utils.hpp"

namespace hatter {
namespace package_set_handler {
TEST(PackageSetSanitizeTest, UnknownValue_POSITIVE) {
    PackageSet               config{PackageSet::PackageType::rpm};
    toml::table              table;
    std::vector<std::string> invalidKeys{"invalidKey1", "invalidKey2", "invalidKey3"};
    for (const auto& invalidKey : invalidKeys) {
        table.insert(std::pair<std::string, std::string>(invalidKey, "randomVal"));
    }

    const auto errors = sanitize(config, table);
    const auto totalUnknownError =
        checkTotalError<UnknownValueError>(errors, [&](UnknownValueError* error) {
            EXPECT_THAT(error->undefinedVals, testing::UnorderedElementsAreArray(invalidKeys));
        });

    EXPECT_EQ(totalUnknownError, 1);
    EXPECT_EQ(checkTotalOtherError<UnknownValueError>(errors), 0);
}
TEST(PackageSetSanitizeTest, UnknownValue_NEGATIVE) {
    PackageSet  config{PackageSet::PackageType::rpm};
    toml::table table;

    auto errors = sanitize(config, table);
    EXPECT_EQ(checkTotalError<UnknownValueError>(errors), 0);
    EXPECT_EQ(checkTotalOtherError<UnknownValueError>(errors), 0);
}

TEST(PackageSetSanitizeTest, InvalidVal_RPM_POSITIVE) {
    PackageSet  config{PackageSet::PackageType::rpm};
    toml::table table;

    config.installList.push_back("bat");
    config.installList.push_back("non_existent_package1");
    config.installList.push_back("non_existent_package2");
    config.installList.push_back("neovim");

    const auto errors = sanitize(config, table);

    const auto totalInvalidValError =
        checkTotalError<InvalidValueError>(errors, [&](InvalidValueError* error) {
            EXPECT_THAT(error->invalidVals,
                        testing::UnorderedElementsAreArray(
                            {"non_existent_package1", "non_existent_package2"}));
        });

    EXPECT_EQ(totalInvalidValError, 1);
    EXPECT_EQ(checkTotalOtherError<InvalidValueError>(errors), 0);
}
TEST(PackageSetSanitizeTest, InvalidVal_RPM_NEGATIVE) {
    PackageSet  config{PackageSet::PackageType::rpm};
    toml::table table;

    config.installList.push_back("bat");
    config.installList.push_back("zzuf");
    config.installList.push_back("neovim");

    const auto errors = sanitize(config, table);

    EXPECT_EQ(checkTotalError<InvalidValueError>(errors), 0);
    EXPECT_EQ(checkTotalOtherError<InvalidValueError>(errors), 0);
}

TEST(PackageSetSanitizeTest, InvalidVal_RPM_GROUP_POSITIVE) {
    PackageSet  config{PackageSet::PackageType::rpm_group};
    toml::table table;

    config.installList.push_back("xmonad-mate");
    config.installList.push_back("non_existent_group1");
    config.installList.push_back("non_existent_group2");
    config.installList.push_back("system-tools");

    const auto errors = sanitize(config, table);

    const auto totalInvalidValError =
        checkTotalError<InvalidValueError>(errors, [&](InvalidValueError* error) {
            EXPECT_THAT(
                error->invalidVals,
                testing::UnorderedElementsAreArray({"non_existent_group1", "non_existent_group2"}));
        });

    EXPECT_EQ(totalInvalidValError, 1);
    EXPECT_EQ(checkTotalOtherError<InvalidValueError>(errors), 0);
}
TEST(PackageSetSanitizeTest, InvalidVal_RPM_GROUP_NEGATIVE) {
    PackageSet  config{PackageSet::PackageType::rpm_group};
    toml::table table;

    config.installList.push_back("xmonad-mate");
    config.installList.push_back("tomcat");
    config.installList.push_back("system-tools");

    const auto errors = sanitize(config, table);

    EXPECT_EQ(checkTotalError<InvalidValueError>(errors), 0);
    EXPECT_EQ(checkTotalOtherError<InvalidValueError>(errors), 0);
}

TEST(PackageSetSanitizeTest, InvalidVal_UnknownVal) {
    toml::table              table;
    std::vector<std::string> invalidKeys{"invalidKey1", "invalidKey2", "invalidKey3"};
    for (const auto& invalidKey : invalidKeys) {
        table.insert(std::pair<std::string, std::string>(invalidKey, "randomVal"));
    }

    PackageSet config{PackageSet::PackageType::rpm_group};
    config.installList.push_back("xmonad-mate");
    config.installList.push_back("non_existent_group2");
    config.installList.push_back("system-tools");

    const auto errors = sanitize(config, table);

    EXPECT_EQ(checkTotalError<InvalidValueError>(errors), 1);
    EXPECT_EQ(checkTotalError<UnknownValueError>(errors), 1);
    EXPECT_EQ(checkTotalOtherError<InvalidValueError>(errors), 1);
}
}  // namespace package_set_handler
}  // namespace hatter
