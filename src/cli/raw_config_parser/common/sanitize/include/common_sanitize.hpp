#pragma once

#include <filesystem>
#include <memory>
#include <string>
#include <vector>

#include "raw_config_parser_utils.hpp"
#include "toml11/toml.hpp"

#include "error_type.hpp"

namespace hatter {
struct UnknownValueError : public HatterParserError {
    std::vector<std::string> undefinedVals;

    std::string what() const override;
};
std::shared_ptr<UnknownValueError> checkUnknownValue(const toml::table& table);

struct NotPositiveError : public SingleKeyError {
    explicit NotPositiveError(const std::string& keyName);
    std::string what() const override;
};
std::shared_ptr<NotPositiveError> checkNotPositive(const std::string& keyName, const int value);

struct InvalidValueError : public HatterParserError {
    std::vector<std::string> invalidVals;
    const std::string        typeName;
    const std::string        extraMessage;

    InvalidValueError(const std::string& typeName, const std::string& extraMessage);
    InvalidValueError(const std::string& typeName,
                      const std::string& invalidVal,
                      const std::string& extraMessage);
    std::string what() const override;
};
std::shared_ptr<InvalidValueError> checkInvalidValue(const std::string&              typeName,
                                                     const std::vector<std::string>& values,
                                                     const std::vector<std::string>& validVals,
                                                     const std::string& extraMessage = "");
std::shared_ptr<InvalidValueError> checkInvalidValue(const std::string&              typeName,
                                                     const std::vector<std::string>& values,
                                                     const std::string&              cmd,
                                                     const std::string& delimiter = "\n");
template <typename TBool, typename T = std::enable_if_t<std::is_same<TBool, bool>{}>>
std::shared_ptr<InvalidValueError> checkInvalidValue(const std::string&              typeName,
                                                     const std::vector<std::string>& values,
                                                     const std::filesystem::path&    filePath,
                                                     const TBool        useRegex     = true,
                                                     const std::string& extraMessage = "") {
    std::shared_ptr<InvalidValueError> error = nullptr;
    for (const auto& value : values) {
        const auto match = ripgrepSearchFile(value, filePath.string(), useRegex);

        if (match < 1) {
            if (!error) { error = std::make_shared<InvalidValueError>(typeName, extraMessage); }
            error->invalidVals.push_back(value);
        }
    }

    return error;
}

struct FileNotExistError : public SingleKeyError {
    std::string fileName;
    std::string what() const override;
    explicit FileNotExistError(const std::string& keyName, const std::string& fileName);
};
std::shared_ptr<FileNotExistError> checkFileNotExist(const std::string&           keyName,
                                                     const std::filesystem::path& filePath);
}  // namespace hatter