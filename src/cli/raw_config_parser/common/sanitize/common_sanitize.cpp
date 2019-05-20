#include "common_sanitize.hpp"

#include "ascii_code.hpp"
#include "utils.hpp"

namespace hatter {
std::string UnknownValueError::what() const {
    const auto undefinedStr = formatStr(strJoin(undefinedVals), ascii_code::kErrorListFormat);
    return formatStr("unknown", ascii_code::kImportantWordFormat) + " key(s): " + undefinedStr;
}
std::shared_ptr<UnknownValueError> checkUnknownValue(const toml::table& table) {
    if (!table.empty()) {
        auto error = std::make_shared<UnknownValueError>();
        for (auto const& [key, val] : table) { error->undefinedVals.push_back(key); }
        return error;
    }
    return nullptr;
}

InvalidValueError::InvalidValueError(const std::string& typeName) : typeName{typeName} {}
InvalidValueError::InvalidValueError(const std::string& typeName, const std::string& invalidVal)
    : typeName{typeName} {
    invalidVals.push_back(invalidVal);
}
std::string InvalidValueError::what() const {
    const auto invalidStr = formatStr(strJoin(invalidVals), ascii_code::kErrorListFormat);
    return "invalid " + formatStr(typeName, ascii_code::kImportantWordFormat) +
           "(s): " + invalidStr;
}
std::shared_ptr<InvalidValueError> checkInvalidValue(const std::string&              typeName,
                                                     const std::string&              value,
                                                     const std::vector<std::string>& validVals) {
    if (!inVector(value, validVals)) {
        return std::make_shared<InvalidValueError>(typeName, value);
    }
    return nullptr;
}
std::shared_ptr<InvalidValueError> checkInvalidValue(const std::string&              typeName,
                                                     const std::vector<std::string>& values,
                                                     const std::vector<std::string>& validVals) {
    std::shared_ptr<InvalidValueError> error = nullptr;

    for (const auto& val : values) {
        if (!inVector(val, validVals)) {
            if (!error) { error = std::make_shared<InvalidValueError>(typeName); }
            error->invalidVals.push_back(val);
        }
    }

    return error;
}

std::shared_ptr<InvalidValueError> checkInvalidValue(const std::string& typeName,
                                                     const std::string& value,
                                                     const std::string& cmd,
                                                     const std::string& delimiter) {
    std::string tempOutput;
    const auto  errCode = execCommand(cmd, tempOutput);

    if (errCode == 0) {
        const auto validVals = strSplit(tempOutput, delimiter);
        return checkInvalidValue(typeName, value, validVals);
    }

    throw std::runtime_error(cmd + " failed with output: " + tempOutput);
    return nullptr;
}
}  // namespace hatter
