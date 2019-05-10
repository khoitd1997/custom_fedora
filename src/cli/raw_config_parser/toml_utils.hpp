#ifndef _TOML_UTILS_HPP
#define _TOML_UTILS_HPP

#include <cxxabi.h>

#include <spdlog/spdlog.h>

#include <memory>
#include <string>
#include <type_traits>
#include <typeinfo>
#include <vector>

#include "raw_config_parser.hpp"
#include "toml11/toml.hpp"

struct TOMLError {
    const std::string keyName;

    virtual std::string what() = 0;
    bool                hasError() const;

    TOMLError(const std::string& keyName);
    virtual ~TOMLError() = 0;
};

struct TOMLWrongTypeError : TOMLError {
    const std::string correctType;

    TOMLWrongTypeError();
    TOMLWrongTypeError(const std::string& keyName, const std::string& correctType);

    std::string what() override;
};

struct TOMLExistentError : TOMLError {
    TOMLExistentError();
    explicit TOMLExistentError(const std::string& keyName);

    std::string what() override;
};

struct TOMLEmptyStringError : TOMLError {
    TOMLEmptyStringError();
    explicit TOMLEmptyStringError(const std::string& keyName);

    std::string what() override;
};

template <typename T>
std::string getTypeName() {
    auto rawName = abi::__cxa_demangle(typeid(T).name(), nullptr, nullptr, nullptr);
    spdlog::error("Trying to get name of unkown type " + std::string(rawName));
    free(rawName);
    exit(1);
}

template <>
std::string getTypeName<int>();

template <>
std::string getTypeName<bool>();

template <>
std::string getTypeName<std::string>();

template <>
std::string getTypeName<hatter::Repo>();

template <>
std::string getTypeName<toml::table>();

template <typename V>
std::string getTypeName(const std::vector<V>& variable) {
    (void)(variable);
    return "array of " + getTypeName<V>();
}

template <typename T>
std::string getTypeName(const T& variable) {
    (void)(variable);
    return getTypeName<T>();
}

namespace internal {
template <typename T>
std::shared_ptr<TOMLError> getTOMLValBase(const toml::table& t,
                                          const std::string& keyName,
                                          T&                 storage,
                                          const bool         isOptional = true) {
    try {
        storage = toml::get<T>(t.at(keyName));
    } catch (const toml::type_error& e) {
        return std::make_shared<TOMLWrongTypeError>(keyName, getTypeName(storage));
    } catch (const std::out_of_range& e) {
        if (!isOptional) { return std::make_shared<TOMLExistentError>(keyName); }
    }
    return std::make_shared<TOMLWrongTypeError>();
}
};  // namespace internal

template <typename T>
std::shared_ptr<TOMLError> getTOMLVal(const toml::table& t,
                                      const std::string& keyName,
                                      T&                 storage,
                                      const bool         isOptional = true) {
    return internal::getTOMLValBase(t, keyName, storage, isOptional);
}

template <>
std::shared_ptr<TOMLError> getTOMLVal(const toml::table& t,
                                      const std::string& keyName,
                                      std::string&       storage,
                                      const bool         isOptional);

template <>
std::shared_ptr<TOMLError> getTOMLVal(const toml::table&        t,
                                      const std::string&        keyName,
                                      std::vector<std::string>& storage,
                                      const bool                isOptional);
#endif