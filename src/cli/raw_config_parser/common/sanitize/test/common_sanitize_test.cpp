#include "common_sanitize.hpp"

#include <gtest/gtest.h>
#include <algorithm>
#include <string>

#include "toml11/toml.hpp"
#include "utils.hpp"

namespace hatter {
TEST(CommonSanitizeTest, UnknownValueTest) {
    {
        /* check multiple unknown values */
        toml::table table;
        table["val1"] = "str1";
        table["val2"] = "str2";
        std::vector<std::string> correctUndefinedVals{"val1", "val2"};
        sortVector(correctUndefinedVals);

        auto error = checkUnknownValue(table);
        ASSERT_EQ(error.operator bool(), true);
        sortVector(error->undefinedVals);
        EXPECT_EQ(error->undefinedVals, correctUndefinedVals);
    }

    {
        /* check no unknown values */
        toml::table table;
        auto        error = checkUnknownValue(table);
        EXPECT_EQ(error.operator bool(), false);
    }
}

TEST(CommonSanitizeTest, SingleInputInvalidValueTest) {
    {
        /* check single input val, positive case */
        std::vector<std::string> correctInvalidVals{"testVal"};
        sortVector(correctInvalidVals);
        const std::string              testType{"testType"};
        const std::vector<std::string> validVals{"listVal1", "listVal2"};

        auto error = checkInvalidValue(testType, correctInvalidVals.at(0), validVals);

        ASSERT_EQ(error.operator bool(), true);
        sortVector(error->invalidVals);
        EXPECT_EQ(error->invalidVals, correctInvalidVals);
        EXPECT_EQ(error->extraMessage, "");
        EXPECT_EQ(error->typeName, testType);
    }

    {
        /* check single input val, negative case */
        const std::string              testType{"randomType"};
        const std::vector<std::string> validVals{"listVal1", "listVal2"};

        auto error = checkInvalidValue(testType, validVals.at(0), validVals);
        EXPECT_EQ(error.operator bool(), false);
    }

    {
        /* check single input val, positive with extra message case */
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
