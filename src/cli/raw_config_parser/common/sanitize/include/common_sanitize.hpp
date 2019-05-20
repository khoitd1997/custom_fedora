#pragma once

#include <memory>
#include <string>
#include <vector>

#include "toml11/toml.hpp"

#include "error_type.hpp"

namespace hatter {
struct UnknownValueError : public HatterParserError {
    std::vector<std::string> undefinedVals;

    std::string what() const override;
};
std::shared_ptr<UnknownValueError> checkUnknownValue(const toml::table& table);

struct InvalidValueError : public HatterParserError {
    std::vector<std::string> invalidVals;
    const std::string        typeName;

    explicit InvalidValueError(const std::string& typeName);
    InvalidValueError(const std::string& typeName, const std::string& invalidVal);
    std::string what() const override;
};
std::shared_ptr<InvalidValueError> checkInvalidValue(const std::string&              typeName,
                                                     const std::string&              value,
                                                     const std::vector<std::string>& validVals);

std::shared_ptr<InvalidValueError> checkInvalidValue(const std::string&              typeName,
                                                     const std::vector<std::string>& values,
                                                     const std::vector<std::string>& validVals);

std::shared_ptr<InvalidValueError> checkInvalidValue(const std::string& typeName,
                                                     const std::string& value,
                                                     const std::string& cmd,
                                                     const std::string& delimiter = "\n");
}  // namespace hatter