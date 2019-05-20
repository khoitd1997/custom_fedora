#include "misc_sanitize.hpp"

#include <gtest/gtest.h>

#include "common_sanitize.hpp"
#include "toml11/toml.hpp"

namespace hatter {
namespace misc_handler {
int targetFunc(int num) { return num + 5; }

TEST(MiscSanitizeTest, UnknownValueTest) {
    MiscConfig  config;
    toml::table table;
    table["rfajsdl"] = "fsklja";

    auto errors          = sanitize(config, table);
    auto hasUnknownError = false;
    for (const auto& error : errors) {
        if (dynamic_cast<UnknownValueError*>(error.get())) { hasUnknownError = true; }
    }
    EXPECT_EQ(hasUnknownError, true);
}
}  // namespace misc_handler
}  // namespace hatter