#include "common_sanitize.hpp"

#include <algorithm>
#include <string>

#include "gmock/gmock.h"

#include "toml11/toml.hpp"
#include "utils.hpp"

namespace hatter {
TEST(CommonSanitizeTest, UnknownValue_POSITIVE_MULTI) {
    toml::table table;
    table["val1"] = "str1";
    table["val2"] = "str2";
    std::vector<std::string> correctUndefinedVals{"val1", "val2"};

    auto error = checkUnknownValue(table);
    ASSERT_EQ(error.operator bool(), true);
    EXPECT_THAT(error->undefinedVals, testing::UnorderedElementsAreArray(correctUndefinedVals));
}
TEST(CommonSanitizeTest, UnknownValue_NEGATIVE) {
    toml::table table;
    auto        error = checkUnknownValue(table);
    EXPECT_EQ(error.operator bool(), false);
}

TEST(CommonSanitizeTest, InvalidValue_SINGLE_INPUT_POSITIVE) {
    std::vector<std::string>       correctInvalidVals{"testVal"};
    const std::string              testType{"testType"};
    const std::vector<std::string> validVals{"allowedVal1", "allowedVal2"};

    auto error = checkInvalidValue(testType, correctInvalidVals.at(0), validVals);

    ASSERT_EQ(error.operator bool(), true);
    EXPECT_THAT(error->invalidVals, testing::UnorderedElementsAreArray(correctInvalidVals));
    EXPECT_EQ(error->extraMessage, "");
    EXPECT_EQ(error->typeName, testType);
}
TEST(CommonSanitizeTest, InvalidValue_SINGLE_INPUT_NEGATIVE) {
    const std::string              testType{"randomType"};
    const std::vector<std::string> validVals{"allowedVal1", "allowedVal2"};

    auto error = checkInvalidValue(testType, validVals.at(0), validVals);
    EXPECT_EQ(error.operator bool(), false);
}
TEST(CommonSanitizeTest, InvalidValue_SINGLE_INPUT_POSITIVE_EXTRA_MSG) {
    std::vector<std::string>       correctInvalidVals{"dalsjjasf"};
    const std::string              testType{"randomType"};
    const std::vector<std::string> validVals{"allowedVal1", "allowedVal2"};
    const std::string              extraMessage{"somebody once told"};

    auto error = checkInvalidValue(testType, correctInvalidVals.at(0), validVals, extraMessage);

    ASSERT_EQ(error.operator bool(), true);
    EXPECT_THAT(error->invalidVals, testing::UnorderedElementsAreArray(correctInvalidVals));
    EXPECT_EQ(error->extraMessage, extraMessage);
    EXPECT_EQ(error->typeName, testType);
}

TEST(CommonSanitizeTest, InvalidValue_MULTI_INPUT_POSITIVE) {
    std::vector<std::string>       inputVals{"testVal", "testVal1", "allowedVal3"};
    std::vector<std::string>       correctInvalidVals{"testVal", "testVal1"};
    const std::string              testType{"testType"};
    const std::string              extraMessage{"message"};
    const std::vector<std::string> validVals{"allowedVal1", "allowedVal2", "allowedVal3"};

    auto error = checkInvalidValue(testType, inputVals, validVals, extraMessage);

    ASSERT_EQ(error.operator bool(), true);
    EXPECT_THAT(error->invalidVals, testing::UnorderedElementsAreArray(correctInvalidVals));
    EXPECT_EQ(error->extraMessage, extraMessage);
    EXPECT_EQ(error->typeName, testType);
}

TEST(CommonSanitizeTest, InvalidValue_MULTI_INPUT_NEGATIVE) {
    std::vector<std::string> inputVals{"allowedVal2", "allowedVal3", "allowedVal1", "allowedVal3"};
    const std::string        testType{"testType"};
    const std::vector<std::string> validVals{"allowedVal1", "allowedVal2", "allowedVal3"};

    auto error = checkInvalidValue(testType, inputVals, validVals);

    EXPECT_EQ(error.operator bool(), false);
}

TEST(CommonSanitizeTest, InvalidValue_SINGLE_INPUT_CMD_POSITIVE) {
    std::vector<std::string> correctInvalidVals{"testVal"};
    const std::string        testType{"testType"};
    const std::string        testCmd{"echo 'allowedVal1\nallowedVal2\nallowedVal3'"};

    auto error = checkInvalidValue(testType, correctInvalidVals.at(0), testCmd);

    ASSERT_EQ(error.operator bool(), true);
    EXPECT_THAT(error->invalidVals, testing::UnorderedElementsAreArray(correctInvalidVals));
    EXPECT_EQ(error->extraMessage, "Run '" + testCmd + "' for full list");
    EXPECT_EQ(error->typeName, testType);
}
TEST(CommonSanitizeTest, InvalidValue_SINGLE_INPUT_CMD_NEGATIVE) {
    const std::string testType{"testType"};
    const std::string testCmd{"echo 'allowedVal1\nallowedVal2\nallowedVal3'"};

    auto error = checkInvalidValue(testType, "allowedVal1", testCmd);

    EXPECT_EQ(error.operator bool(), false);
}
TEST(CommonSanitizeTest, InvalidValue_SINGLE_INPUT_CMD_POSITIVE_CUSTOM_DELIM) {
    std::vector<std::string> correctInvalidVals{"testVal"};
    const std::string        testType{"testType"};
    const std::string        testCmd{"echo 'allowedVal1, allowedVal2, allowedVal3'"};

    auto error = checkInvalidValue(testType, correctInvalidVals.at(0), testCmd, ", ");

    ASSERT_EQ(error.operator bool(), true);
    EXPECT_THAT(error->invalidVals, testing::UnorderedElementsAreArray(correctInvalidVals));
    EXPECT_EQ(error->extraMessage, "Run '" + testCmd + "' for full list");
    EXPECT_EQ(error->typeName, testType);
}
TEST(CommonSanitizeTest, InvalidValue_SINGLE_INPUT_CMD_NEGATIVE_CUSTOM_DELIM) {
    const std::string testType{"testType"};
    const std::string testCmd{"echo 'allowedVal1, allowedVal2, allowedVal3'"};

    auto error = checkInvalidValue(testType, "allowedVal1", testCmd, ", ");

    EXPECT_EQ(error.operator bool(), false);
}
}  // namespace hatter
