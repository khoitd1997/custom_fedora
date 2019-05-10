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

TOMLWrongTypeError::TOMLWrongTypeError() : TOMLError{""} {}
TOMLWrongTypeError::TOMLWrongTypeError(const std::string& keyName, const std::string& correctType)
    : TOMLError{keyName}, correctType{correctType} {}
std::string TOMLWrongTypeError::what() { return keyName + " should have type " + correctType; }

TOMLExistentError::TOMLExistentError() : TOMLError{""} {}
TOMLExistentError::TOMLExistentError(const std::string& keyName) : TOMLError{keyName} {}
std::string TOMLExistentError::what() { return keyName + " needs to be defined"; }
