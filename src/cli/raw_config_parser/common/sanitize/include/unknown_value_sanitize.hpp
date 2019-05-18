#pragma once

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "toml11/toml.hpp"

#include "error_type.hpp"

namespace hatter {
struct UnknownValueError : public HatterParserError {
    std::vector<std::string> undefinedVals;

    std::string what() const override;
};

std::optional<std::shared_ptr<UnknownValueError>> checkUnknownValue(const toml::table& table);
}  // namespace hatter