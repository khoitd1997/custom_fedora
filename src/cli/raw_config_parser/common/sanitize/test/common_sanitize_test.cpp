#include "common_sanitize.hpp"

#include <gtest/gtest.h>
#include <algorithm>
#include <string>

#include "toml11/toml.hpp"
#include "utils.hpp"

namespace hatter {
TEST(CommonSanitizeTest, UnknownValueTest) {
    {
        toml::table nonEmptyTable;
        nonEmptyTable["val1"] = "str1";
        nonEmptyTable["val2"] = "str2";
        std::vector<std::string> correctUndefinedVals{"val1", "val2"};
        sortVector(correctUndefinedVals);

        auto error = checkUnknownValue(nonEmptyTable);
        ASSERT_EQ(error.operator bool(), true);
        sortVector(error->undefinedVals);
        EXPECT_EQ(error->undefinedVals, correctUndefinedVals);
    }

    {
        toml::table emptyTable;
        auto        error = checkUnknownValue(emptyTable);
        EXPECT_EQ(error.operator bool(), false);
    }
}

TEST(CommonSanitizeTest, SingleInvalidValueTest) {
    {
        std::vector<std::string> correctInvalidVals{"testVal"};
        sortVector(correctInvalidVals);
        const std::string testType{"testType"};

        auto error = checkInvalidValue(
            testType, correctInvalidVals.at(0), std::vector<std::string>{"listVal1", "listVal2"});

        ASSERT_EQ(error.operator bool(), true);
        sortVector(error->invalidVals);
        EXPECT_EQ(error->invalidVals, correctInvalidVals);
        EXPECT_EQ(error->extraMessage, "");
        EXPECT_EQ(error->typeName, testType);
    }

    {
        const std::string              testType{"randomType"};
        const std::vector<std::string> validVals{"listVal1", "listVal2"};

        auto error = checkInvalidValue(testType, validVals.at(0), validVals);
        EXPECT_EQ(error.operator bool(), false);
    }

    {
        std::vector<std::string>       correctInvalidVals{"dalsjjasf"};
        const std::string              testType{"randomType"};
        const std::vector<std::string> validVals{"listVal1", "listVal2"};
        const std::string              extraMessage{"somebody once told"};

        auto error = checkInvalidValue(testType, correctInvalidVals.at(0), validVals, extraMessage);

        ASSERT_EQ(error.operator bool(), true);
        sortVector(error->invalidVals);
        EXPECT_EQ(error->invalidVals, correctInvalidVals);
        EXPECT_EQ(error->extraMessage, extraMessage);
        EXPECT_EQ(error->typeName, testType);
    }
}
}  // namespace hatter
