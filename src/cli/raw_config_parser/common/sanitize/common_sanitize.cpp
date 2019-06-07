#include "common_sanitize.hpp"

#include "formatter.hpp"
#include "utils.hpp"

namespace hatter {
std::string UnknownValueError::what() const {
    const auto undefinedStr = formatter::formatErrorText(strJoin(undefinedVals));
    return formatter::formatImportantText("unknown") + " key(s): " + undefinedStr;
}
std::shared_ptr<UnknownValueError> checkUnknownValue(const toml::table& table) {
    if (!table.empty()) {
        auto error = std::make_shared<UnknownValueError>();
        for (auto const& [key, val] : table) { error->undefinedVals.push_back(key); }
        return error;
    }
    return nullptr;
}

NotPositiveError::NotPositiveError(const std::string& keyName) : SingleKeyError{keyName} {}
std::string NotPositiveError::what() const {
    return "key must be " + formatter::formatImportantText("positive") + ": " +
           formatter::formatErrorText(keyName);
}
std::shared_ptr<NotPositiveError> checkNotPositive(const std::string& keyName, const int value) {
    if (value <= 0) { return std::make_shared<NotPositiveError>(keyName); }
    return nullptr;
}

InvalidValueError::InvalidValueError(const std::string& typeName, const std::string& extraMessage)
    : typeName{typeName}, extraMessage{extraMessage} {}
InvalidValueError::InvalidValueError(const std::string& typeName,
                                     const std::string& invalidVal,
                                     const std::string& extraMessage)
    : typeName{typeName}, extraMessage{extraMessage} {
    invalidVals.push_back(invalidVal);
}
std::string InvalidValueError::what() const {
    const auto invalidStr = formatter::formatErrorText(strJoin(invalidVals));
    return "invalid " + formatter::formatImportantText(typeName) + "(s): " + invalidStr +
           ((extraMessage.empty() ? "" : ". " + extraMessage));
}
std::shared_ptr<InvalidValueError> checkInvalidValue(const std::string&              typeName,
                                                     const std::vector<std::string>& values,
                                                     const std::vector<std::string>& validVals,
                                                     const std::string&              extraMessage) {
    std::shared_ptr<InvalidValueError> error = nullptr;

    for (const auto& val : values) {
        if (!inVector(val, validVals)) {
            if (!error) { error = std::make_shared<InvalidValueError>(typeName, extraMessage); }
            error->invalidVals.push_back(val);
        }
    }

    return error;
}
std::shared_ptr<InvalidValueError> checkInvalidValue(const std::string&              typeName,
                                                     const std::vector<std::string>& values,
                                                     const std::string&              cmd,
                                                     const std::string&              delimiter) {
    std::string tempOutput;
    const auto  errCode = execCommand(cmd, tempOutput);

    if (errCode == 0) {
        const auto validVals = strSplit(tempOutput, delimiter);
        return checkInvalidValue(typeName, values, validVals, "Run '" + cmd + "' for full list");
    }

    throw std::runtime_error(cmd + " failed with output: " + tempOutput);
    return nullptr;
}

FileNotExistError::FileNotExistError(const std::string& keyName) : SingleKeyError{keyName} {}
std::string FileNotExistError::what() const {
    return formatter::formatImportantText(keyName) +
           "(s) don't exist: " + formatter::formatErrorText(strJoin(fileNames));
}
std::shared_ptr<FileNotExistError> checkFileNotExist(
    const std::string& keyName, const std::vector<std::filesystem::path>& filePaths) {
    std::shared_ptr<FileNotExistError> error = nullptr;
    for (const auto& filePath : filePaths) {
        if (!std::filesystem::exists(filePath)) {
            if (!error) { error = std::make_shared<FileNotExistError>(keyName); }
            error->fileNames.push_back(filePath.string());
        }
    }
    return error;
}
}  // namespace hatter