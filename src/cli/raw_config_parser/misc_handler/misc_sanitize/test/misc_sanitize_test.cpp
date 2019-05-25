#include "misc_sanitize.hpp"

#include <gtest/gtest.h>

#include "common_sanitize.hpp"
#include "toml11/toml.hpp"

namespace hatter {
namespace misc_handler {
class MiscSanitizeTest : public ::testing::Test {
   protected:
    // void        SetUp() override {}

    MiscConfig  config;
    toml::table table;
    // void TearDown() override {}
};

TEST_F(MiscSanitizeTest, UnknownValueTest) {
    table["rfajsdl"] = "fsklja";

    auto errors            = sanitize(config, table);
    auto hasUnknownError   = false;
    auto totalUnknownError = 0;
    for (const auto& error : errors) {
        if (dynamic_cast<UnknownValueError*>(error.get())) {
            hasUnknownError = true;
            ++totalUnknownError;
        }
    }
    EXPECT_EQ(hasUnknownError, true);
    EXPECT_EQ(totalUnknownError, 1);
}
}  // namespace misc_handler
}  // namespace hatter
