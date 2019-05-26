#include "misc_sanitize.hpp"

#include <map>

#include "gmock/gmock.h"

#include "common_sanitize.hpp"
#include "raw_config_parser_test_utils.hpp"
#include "toml11/toml.hpp"
#include "utils.hpp"

namespace hatter {
namespace misc_handler {
class MiscSanitizeTest : public testing::Test {
   protected:
    // void SetUp() override {
    // }
    // void TearDown() override {}
    MiscConfig  config;
    toml::table table;
};

TEST_F(MiscSanitizeTest, UnknownValue_POSITIVE) {
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
}
TEST_F(MiscSanitizeTest, UnknownValue_NEGATIVE) {
    auto errors = sanitize(config, table);
    EXPECT_EQ(checkTotalError<UnknownValueError>(errors), 0);
}

TEST_F(MiscSanitizeTest, InvalidValue_LANGUAGE_POSITIVE) {
    config.language = "random_lang";

    const auto errors = sanitize(config, table);
    const auto totalInvalidError =
        checkTotalError<InvalidValueError>(errors, [&](InvalidValueError* error) {
            EXPECT_EQ(error->typeName, "language");
            EXPECT_THAT(error->invalidVals, testing::UnorderedElementsAreArray({config.language}));
        });
    EXPECT_EQ(totalInvalidError, 1);
}
TEST_F(MiscSanitizeTest, InvalidValue_LANGUAGE_NEGATIVE) {
    config.language = "zu_ZA.utf8";

    const auto errors = sanitize(config, table);
    EXPECT_EQ(checkTotalError<InvalidValueError>(errors), 0);
}

TEST_F(MiscSanitizeTest, InvalidValue_TIMEZONE_POSITIVE) {
    config.timezone = "random_time";
    std::vector<std::string> correctInvalidVals;
    correctInvalidVals.push_back(config.timezone);

    const auto errors = sanitize(config, table);
    const auto totalInvalidError =
        checkTotalError<InvalidValueError>(errors, [&](InvalidValueError* error) {
            EXPECT_EQ(error->typeName, "timezone");
            EXPECT_THAT(error->invalidVals, testing::UnorderedElementsAreArray({config.timezone}));
        });
    EXPECT_EQ(totalInvalidError, 1);
}
TEST_F(MiscSanitizeTest, InvalidValue_TIMEZONE_NEGATIVE) {
    config.timezone = "US/Pacific";

    const auto errors = sanitize(config, table);
    EXPECT_EQ(checkTotalError<InvalidValueError>(errors), 0);
}

TEST_F(MiscSanitizeTest, InvalidValue_KEYBOARD_POSITIVE) {
    config.keyboard = "random_keyboard";

    const auto errors = sanitize(config, table);
    const auto totalInvalidError =
        checkTotalError<InvalidValueError>(errors, [&](InvalidValueError* error) {
            EXPECT_EQ(error->typeName, "keyboard");
            EXPECT_THAT(error->invalidVals, testing::UnorderedElementsAreArray({config.keyboard}));
        });
    EXPECT_EQ(totalInvalidError, 1);
}
TEST_F(MiscSanitizeTest, InvalidValue_KEYBOARD_NEGATIVE) {
    config.keyboard = "us";

    const auto errors = sanitize(config, table);
    EXPECT_EQ(checkTotalError<InvalidValueError>(errors), 0);
}

TEST_F(MiscSanitizeTest, InvalidValue_COMBINE_LANGUAGE_KEYBOARD) {
    config.language = "random_lang";
    config.keyboard = "random_keyboard";

    const auto errors = sanitize(config, table);
    const auto totalInvalidError =
        checkTotalError<InvalidValueError>(errors, [&](InvalidValueError* error) {
            if (error->typeName == "keyboard") {
                EXPECT_THAT(error->invalidVals,
                            testing::UnorderedElementsAreArray({config.keyboard}));
            } else if (error->typeName == "language") {
                EXPECT_THAT(error->invalidVals,
                            testing::UnorderedElementsAreArray({config.language}));
            } else {
                ADD_FAILURE();
            }
        });
    EXPECT_EQ(totalInvalidError, 2);
}
TEST_F(MiscSanitizeTest, InvalidValue_COMBINE_ALL) {
    config.timezone = "random_tz";
    config.language = "random_lang";
    config.keyboard = "random_keyboard";

    const auto errors = sanitize(config, table);
    const auto totalInvalidError =
        checkTotalError<InvalidValueError>(errors, [&](InvalidValueError* error) {
            if (error->typeName == "keyboard") {
                EXPECT_THAT(error->invalidVals,
                            testing::UnorderedElementsAreArray({config.keyboard}));
            } else if (error->typeName == "language") {
                EXPECT_THAT(error->invalidVals,
                            testing::UnorderedElementsAreArray({config.language}));
            } else if (error->typeName == "timezone") {
                EXPECT_THAT(error->invalidVals,
                            testing::UnorderedElementsAreArray({config.timezone}));
            } else {
                ADD_FAILURE();
            }
        });
    EXPECT_EQ(totalInvalidError, 3);
}
}  // namespace misc_handler
}  // namespace hatter
