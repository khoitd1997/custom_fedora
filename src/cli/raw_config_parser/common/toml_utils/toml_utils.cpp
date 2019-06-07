#include "toml_utils.hpp"

#include <algorithm>

#include "formatter.hpp"
#include "utils.hpp"

namespace hatter {
template <>
std::string getTypeName<int>() {
    return "integer";
}

template <>
std::string getTypeName<bool>() {
    return "boolean";
}

template <>
std::string getTypeName<std::string>() {
    return "string";
}

template <>
std::string getTypeName<hatter::CustomRepo>() {
    return "repo";
}

template <>
std::string getTypeName<toml::table>() {
    return "table";
}

TOMLError::TOMLError(const std::string& keyName) : keyName{keyName} {}
TOMLError::~TOMLError() {}

TOMLTypeError::TOMLTypeError(const std::string& keyName, const std::string& correctType)
    : TOMLError{keyName}, correctType{correctType} {}
std::string TOMLTypeError::what() const {
    return "type " + formatter::formatImportantText(correctType) +
           " is needed for: " + formatter::formatErrorText(keyName);
}

TOMLExistentError::TOMLExistentError(const std::string& keyName) : TOMLError{keyName} {}
std::string TOMLExistentError::what() const {
    return formatter::formatImportantText(keyName) + " needs to be defined";
}

TOMLEmptyStringError::TOMLEmptyStringError(const std::string& keyName) : TOMLError{keyName} {}
std::string TOMLEmptyStringError::what() const {
    return "key has " + formatter::formatImportantText("empty") +
           " value: " + formatter::formatErrorText(keyName);
}

std::shared_ptr<TOMLError> getBaseTable(toml::table&             t,
                                        const ConfigSectionBase& confSection,
                                        toml::table&             out) {
    return getNonMemberTOMLVal(t, confSection.keyName, out);
}
std::shared_ptr<TOMLError> getBaseTable(toml::table&              t,
                                        const ConfigSectionBase&  confSection,
                                        std::vector<toml::table>& out) {
    return getNonMemberTOMLVal(t, confSection.keyName, out);
}

template <>
std::shared_ptr<TOMLError> getTOMLVal(toml::table& t, ConfigMember<std::string>& conf) {
    internal::TOMLValStatus status;
    auto                    error = internal::getTOMLValBase(t, conf, status);

    if (!error) {
        if (status == internal::TOMLValStatus::PRESENT && conf.value.empty()) {
            return std::make_shared<TOMLEmptyStringError>(conf.keyName);
        }
    }
    return error;
}

template <>
std::shared_ptr<TOMLError> getTOMLVal(toml::table&                            t,
                                      ConfigMember<std::vector<std::string>>& conf) {
    auto error = internal::getTOMLValBase(t, conf);
    if (!error && !(conf.value.empty())) {
        if (std::any_of(conf.value.cbegin(), conf.value.cend(), [](std::string str) -> bool {
                return str.empty();
            })) {
            return std::make_shared<TOMLEmptyStringError>(conf.keyName);
        }
    }
    return error;
}

std::shared_ptr<TOMLError> getTOMLVal(toml::table&                                      rawConf,
                                      ConfigMember<std::vector<std::filesystem::path>>& conf,
                                      const std::filesystem::path&                      parentDir) {
    ConfigMember<std::vector<std::string>> tempConf{conf.keyName, .isOptional = conf.isOptional};
    auto                                   error = getTOMLVal(rawConf, tempConf);

    if (!error) {
        for (const auto& tempStr : tempConf.value) { conf.value.push_back(parentDir / tempStr); }
    }
    return error;
}
}  // namespace hatter