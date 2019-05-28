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
    return "invalid " + formatter::formatImportantText(typeName) + "(s): " + invalidStr + ". " +
           extraMessage;
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

FileNotExistError::FileNotExistError(const std::string fileName) : fileName{fileName} {}
std::string FileNotExistError::what() const {
    return formatter::formatImportantText("file ") +
           "doesn't exist: " + formatter::formatErrorText(fileName);
}
std::shared_ptr<FileNotExistError> checkFileNotExist(const std::filesystem::path& filePath) {
    if (std::filesystem::exists(filePath)) { return nullptr; }
    return std::make_shared<FileNotExistError>(filePath.filename().string());
}
}  // namespace hatter
