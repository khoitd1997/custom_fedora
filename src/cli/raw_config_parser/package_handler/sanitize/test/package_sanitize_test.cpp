#include "package_sanitize.hpp"

#include <map>

#include "gmock/gmock.h"

#include "common_sanitize.hpp"
#include "raw_config_parser_test_utils.hpp"
#include "toml11/toml.hpp"
#include "utils.hpp"

namespace hatter {
namespace package_handler {
class PackageSanitizeTest : public testing::Test {
   protected:
    PackageConfig config;
    toml::table   table;
};

TEST_F(PackageSanitizeTest, UnknownValue_POSITIVE) {
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
TEST_F(PackageSanitizeTest, UnknownValue_NEGATIVE) {
    auto errors = sanitize(config, table);
    EXPECT_EQ(checkTotalError<UnknownValueError>(errors), 0);
    EXPECT_EQ(checkTotalOtherError<UnknownValueError>(errors), 0);
}
}  // namespace package_handler
}  // namespace hatter
