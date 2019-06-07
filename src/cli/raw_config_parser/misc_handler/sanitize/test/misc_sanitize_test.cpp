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
    EXPECT_EQ(checkTotalOtherError<UnknownValueError>(errors), 0);
}
TEST_F(MiscSanitizeTest, UnknownValue_NEGATIVE) {
    auto errors = sanitize(config, table);

    EXPECT_EQ(checkTotalError<UnknownValueError>(errors), 0);
    EXPECT_EQ(checkTotalOtherError<UnknownValueError>(errors), 0);
}

TEST_F(MiscSanitizeTest, InvalidValue_LANGUAGE_POSITIVE) {
    config.language.value = "random_lang";

    const auto errors = sanitize(config, table);
    const auto totalInvalidError =
        checkTotalError<InvalidValueError>(errors, [&](InvalidValueError* error) {
            EXPECT_EQ(error->typeName, "language");
            EXPECT_THAT(error->invalidVals,
                        testing::UnorderedElementsAreArray({config.language.value}));
        });

    EXPECT_EQ(totalInvalidError, 1);
    EXPECT_EQ(checkTotalOtherError<InvalidValueError>(errors), 0);
}
TEST_F(MiscSanitizeTest, InvalidValue_LANGUAGE_NEGATIVE) {
    config.language.value = "zu_ZA.utf8";

    const auto errors = sanitize(config, table);

    EXPECT_EQ(checkTotalError<InvalidValueError>(errors), 0);
    EXPECT_EQ(checkTotalOtherError<InvalidValueError>(errors), 0);
}

TEST_F(MiscSanitizeTest, InvalidValue_TIMEZONE_POSITIVE) {
    config.timezone.value = "random_time";
    std::vector<std::string> correctInvalidVals;
    correctInvalidVals.push_back(config.timezone.value);

    const auto errors = sanitize(config, table);
    const auto totalInvalidError =
        checkTotalError<InvalidValueError>(errors, [&](InvalidValueError* error) {
            EXPECT_EQ(error->typeName, "timezone");
            EXPECT_THAT(error->invalidVals,
                        testing::UnorderedElementsAreArray({config.timezone.value}));
        });

    EXPECT_EQ(totalInvalidError, 1);
    EXPECT_EQ(checkTotalOtherError<InvalidValueError>(errors), 0);
}
TEST_F(MiscSanitizeTest, InvalidValue_TIMEZONE_NEGATIVE) {
    config.timezone.value = "US/Pacific";

    const auto errors = sanitize(config, table);

    EXPECT_EQ(checkTotalError<InvalidValueError>(errors), 0);
    EXPECT_EQ(checkTotalOtherError<InvalidValueError>(errors), 0);
}

TEST_F(MiscSanitizeTest, InvalidValue_KEYBOARD_POSITIVE) {
    config.keyboard.value = "random_keyboard";

    const auto errors = sanitize(config, table);
    const auto totalInvalidError =
        checkTotalError<InvalidValueError>(errors, [&](InvalidValueError* error) {
            EXPECT_EQ(error->typeName, "keyboard");
            EXPECT_THAT(error->invalidVals,
                        testing::UnorderedElementsAreArray({config.keyboard.value}));
        });

    EXPECT_EQ(totalInvalidError, 1);
    EXPECT_EQ(checkTotalOtherError<InvalidValueError>(errors), 0);
}
TEST_F(MiscSanitizeTest, InvalidValue_KEYBOARD_NEGATIVE) {
    config.keyboard.value = "us";

    const auto errors = sanitize(config, table);

    EXPECT_EQ(checkTotalError<InvalidValueError>(errors), 0);
    EXPECT_EQ(checkTotalOtherError<InvalidValueError>(errors), 0);
}

TEST_F(MiscSanitizeTest, InvalidValue_COMBINE_LANGUAGE_KEYBOARD) {
    config.language.value = "random_lang";
    config.keyboard.value = "random_keyboard";

    const auto errors = sanitize(config, table);
    const auto totalInvalidError =
        checkTotalError<InvalidValueError>(errors, [&](InvalidValueError* error) {
            if (error->typeName == "keyboard") {
                EXPECT_THAT(error->invalidVals,
                            testing::UnorderedElementsAreArray({config.keyboard.value}));
            } else if (error->typeName == "language") {
                EXPECT_THAT(error->invalidVals,
                            testing::UnorderedElementsAreArray({config.language.value}));
            } else {
                ADD_FAILURE();
            }
        });

    EXPECT_EQ(totalInvalidError, 2);
    EXPECT_EQ(checkTotalOtherError<InvalidValueError>(errors), 0);
}
TEST_F(MiscSanitizeTest, InvalidValue_COMBINE_ALL) {
    config.timezone.value = "random_tz";
    config.language.value = "random_lang";
    config.keyboard.value = "random_keyboard";

    const auto errors = sanitize(config, table);
    const auto totalInvalidError =
        checkTotalError<InvalidValueError>(errors, [&](InvalidValueError* error) {
            if (error->typeName == "keyboard") {
                EXPECT_THAT(error->invalidVals,
                            testing::UnorderedElementsAreArray({config.keyboard.value}));
            } else if (error->typeName == "language") {
                EXPECT_THAT(error->invalidVals,
                            testing::UnorderedElementsAreArray({config.language.value}));
            } else if (error->typeName == "timezone") {
                EXPECT_THAT(error->invalidVals,
                            testing::UnorderedElementsAreArray({config.timezone.value}));
            } else {
                ADD_FAILURE();
            }
        });
    EXPECT_EQ(totalInvalidError, 3);

    const auto totalUnknownError = checkTotalError<UnknownValueError>(errors);
    EXPECT_EQ(totalUnknownError, 0);
    EXPECT_EQ(checkTotalOtherError<InvalidValueError>(errors), 0);
}

TEST_F(MiscSanitizeTest, AllError) {
    table["rand_key1"]    = "rand_val1";
    config.timezone.value = "random_tz";
    config.language.value = "random_lang";
    config.keyboard.value = "random_keyboard";

    const auto errors = sanitize(config, table);
    const auto totalInvalidError =
        checkTotalError<InvalidValueError>(errors, [&](InvalidValueError* error) {
            if (error->typeName == "keyboard") {
                EXPECT_THAT(error->invalidVals,
                            testing::UnorderedElementsAreArray({config.keyboard.value}));
            } else if (error->typeName == "language") {
                EXPECT_THAT(error->invalidVals,
                            testing::UnorderedElementsAreArray({config.language.value}));
            } else if (error->typeName == "timezone") {
                EXPECT_THAT(error->invalidVals,
                            testing::UnorderedElementsAreArray({config.timezone.value}));
            } else {
                ADD_FAILURE();
            }
        });

    EXPECT_EQ(totalInvalidError, 3);
    EXPECT_EQ(checkTotalOtherError<InvalidValueError>(errors), 1);
    EXPECT_EQ(checkTotalError<UnknownValueError>(errors), 1);
}
}  // namespace misc_handler
}  // namespace hatter
