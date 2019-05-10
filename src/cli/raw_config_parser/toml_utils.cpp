#include "toml_utils.hpp"

#include <algorithm>

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
std::string getTypeName<hatter::Repo>() {
    return "repo";
}

template <>
std::string getTypeName<toml::table>() {
    return "table";
}

TOMLError::TOMLError(const std::string& keyName) : keyName{keyName} {}
TOMLError::~TOMLError() {}
bool TOMLError::hasError() const { return !keyName.empty(); }

TOMLTypeError::TOMLTypeError(const std::string& keyName, const std::string& correctType)
    : TOMLError{keyName}, correctType{correctType} {}
std::string TOMLTypeError::what() { return keyName + " should have type " + correctType; }

TOMLExistentError::TOMLExistentError(const std::string& keyName) : TOMLError{keyName} {}
std::string TOMLExistentError::what() { return keyName + " needs to be defined"; }

TOMLEmptyStringError::TOMLEmptyStringError(const std::string& keyName) : TOMLError{keyName} {}
std::string TOMLEmptyStringError::what() { return keyName + " has empty value"; }

TOMLNoError::TOMLNoError(const bool isPresent) : TOMLError{""}, isPresent(isPresent) {}
std::string TOMLNoError::what() { return ""; }

template <>
std::shared_ptr<TOMLError> getTOMLVal(toml::table&       t,
                                      const std::string& keyName,
                                      std::string&       storage,
                                      const bool         isOptional) {
    auto ret = internal::getTOMLValBase(t, keyName, storage, isOptional);

    if (auto noErrError = dynamic_cast<TOMLNoError*>(ret.get())) {
        if (noErrError->isPresent && storage.empty()) {
            return std::make_shared<TOMLEmptyStringError>(keyName);
        }
    }
    return ret;
}

std::shared_ptr<TOMLError> getTOMLVal(const toml::table& t,
                                      const std::string& keyName,
                                      toml::table&       storage,
                                      const bool         isOptional) {
    const auto status = internal::getTOMLValHelper(t, keyName, storage);
    return internal::getTOMLErrorPtr(status, keyName, storage, isOptional);
}

template <>
std::shared_ptr<TOMLError> getTOMLVal(toml::table&              t,
                                      const std::string&        keyName,
                                      std::vector<std::string>& storage,
                                      const bool                isOptional) {
    auto error = internal::getTOMLValBase(t, keyName, storage, isOptional);
    if (!(storage.empty())) {
        if (std::any_of(storage.cbegin(), storage.cend(), [](std::string str) -> bool {
                return str.empty();
            })) {
            return std::make_shared<TOMLEmptyStringError>(keyName);
        }
    }
    return error;
}