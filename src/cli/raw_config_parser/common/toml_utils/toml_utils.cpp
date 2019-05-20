#include "toml_utils.hpp"

#include <algorithm>

#include "ascii_format.hpp"
#include "utils.hpp"

namespace hatter {
namespace {
static const auto keyNameFormat = ascii_format::kImportantWordFormat;
}  // namespace

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
    return formatStr(keyName, keyNameFormat) + " should have type " +
           formatStr(correctType, ascii_format::kImportantWordFormat);
}

TOMLExistentError::TOMLExistentError(const std::string& keyName) : TOMLError{keyName} {}
std::string TOMLExistentError::what() const {
    return formatStr(keyName, keyNameFormat) + " needs to be defined";
}

TOMLEmptyStringError::TOMLEmptyStringError(const std::string& keyName) : TOMLError{keyName} {}
std::string TOMLEmptyStringError::what() const {
    return "key has " + formatStr("empty", ascii_format::kImportantWordFormat) + " value";
}

template <>
std::shared_ptr<TOMLError> getTOMLVal(toml::table&       t,
                                      const std::string& keyName,
                                      std::string&       storage,
                                      const bool         isOptional) {
    auto status = internal::getTOMLValHelper(t, keyName, storage);
    auto error  = internal::getTOMLErrorPtr(status, keyName, storage, isOptional);

    if (!error) {
        if (status == internal::TOMLValStatus::PRESENT && storage.empty()) {
            return std::make_shared<TOMLEmptyStringError>(keyName);
        }
    }
    return error;
}

template <>
std::shared_ptr<TOMLError> getTOMLVal(toml::table&              t,
                                      const std::string&        keyName,
                                      std::vector<std::string>& storage,
                                      const bool                isOptional) {
    auto error = internal::getTOMLValBase(t, keyName, storage, isOptional);
    if (!error && !(storage.empty())) {
        if (std::any_of(storage.cbegin(), storage.cend(), [](std::string str) -> bool {
                return str.empty();
            })) {
            return std::make_shared<TOMLEmptyStringError>(keyName);
        }
    }
    return error;
}
}  // namespace hatter