#include "toml_utils.hpp"

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

TOMLWrongTypeError::TOMLWrongTypeError() : TOMLError{""} {}
TOMLWrongTypeError::TOMLWrongTypeError(const std::string& keyName, const std::string& correctType)
    : TOMLError{keyName}, correctType{correctType} {}
std::string TOMLWrongTypeError::what() { return keyName + " should have type " + correctType; }

TOMLExistentError::TOMLExistentError() : TOMLError{""} {}
TOMLExistentError::TOMLExistentError(const std::string& keyName) : TOMLError{keyName} {}
std::string TOMLExistentError::what() { return keyName + " needs to be defined"; }

TOMLEmptyStringError::TOMLEmptyStringError() : TOMLError{""} {}
TOMLEmptyStringError::TOMLEmptyStringError(const std::string& keyName) : TOMLError{keyName} {}
std::string TOMLEmptyStringError::what() { return keyName + " has empty value"; }

template <>
std::shared_ptr<TOMLError> getTOMLVal(const toml::table& t,
                                      const std::string& keyName,
                                      std::string&       storage,
                                      const bool         isOptional) {
    try {
        storage = toml::get<std::string>(t.at(keyName));
        if (storage.empty()) { return std::make_shared<TOMLEmptyStringError>(keyName); }
    } catch (const toml::type_error& e) {
        return std::make_shared<TOMLWrongTypeError>(keyName, getTypeName(storage));
    } catch (const std::out_of_range& e) {
        if (!isOptional) { return std::make_shared<TOMLExistentError>(keyName); }
    }
    return std::make_shared<TOMLWrongTypeError>();
}

template <>
std::shared_ptr<TOMLError> getTOMLVal(const toml::table&        t,
                                      const std::string&        keyName,
                                      std::vector<std::string>& storage,
                                      const bool                isOptional) {
    auto error = internal::getTOMLValBase(t, keyName, storage, isOptional);
    if (!(storage.empty())) {
        for (const auto& str : storage) {
            if (str.empty()) { return std::make_shared<TOMLEmptyStringError>(keyName); }
        }
    }
    return error;
}