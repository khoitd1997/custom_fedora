#pragma once

#include <cxxabi.h>

#include <spdlog/spdlog.h>

#include <memory>
#include <optional>
#include <string>
#include <type_traits>
#include <typeinfo>
#include <vector>

#include "toml11/toml.hpp"

#include "error_type.hpp"
#include "hatter_config_type.hpp"

namespace hatter {
struct TOMLError : HatterParserError {
    const std::string keyName;

    explicit TOMLError(const std::string& keyName);
    virtual ~TOMLError();
};

struct TOMLTypeError : TOMLError {
    const std::string correctType;

    TOMLTypeError(const std::string& keyName, const std::string& correctType);

    std::string what() const override;
};

struct TOMLExistentError : TOMLError {
    explicit TOMLExistentError(const std::string& keyName);

    std::string what() const override;
};

struct TOMLEmptyStringError : TOMLError {
    explicit TOMLEmptyStringError(const std::string& keyName);

    std::string what() const override;
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
enum class TOMLValStatus { NOT_PRESENT, PRESENT, PRESENT_TYPE_ERROR };
template <typename T>
TOMLValStatus getTOMLValHelper(toml::table& t, const std::string& keyName, T& storage) {
    auto ret = TOMLValStatus::PRESENT;
    try {
        storage = toml::get<T>(t.at(keyName));
    } catch (const toml::type_error& e) {
        ret = TOMLValStatus::PRESENT_TYPE_ERROR;
    } catch (const std::out_of_range& e) { ret = TOMLValStatus::NOT_PRESENT; }
    t.erase(keyName);
    return ret;
}

template <typename T>
std::optional<std::shared_ptr<TOMLError>> getTOMLErrorPtr(const TOMLValStatus status,
                                                          const std::string&  keyName,
                                                          const T&            storage,
                                                          const bool          isOptional = true) {
    switch (status) {
        case TOMLValStatus::NOT_PRESENT:
            if (isOptional) { return {}; }
            return std::make_shared<TOMLExistentError>(keyName);
            break;
        case TOMLValStatus::PRESENT:
            return {};
            break;
        case TOMLValStatus::PRESENT_TYPE_ERROR:
            return std::make_shared<TOMLTypeError>(keyName, getTypeName(storage));
            break;
        default:
            return {};
            break;
    }
}

template <typename T>
std::optional<std::shared_ptr<TOMLError>> getTOMLValBase(toml::table&       t,
                                                         const std::string& keyName,
                                                         T&                 storage,
                                                         const bool         isOptional = true) {
    auto status = getTOMLValHelper(t, keyName, storage);
    return getTOMLErrorPtr(status, keyName, storage, isOptional);
}
};  // namespace internal

template <typename T>
std::optional<std::shared_ptr<TOMLError>> getTOMLVal(toml::table&       t,
                                                     const std::string& keyName,
                                                     T&                 storage,
                                                     const bool         isOptional = true) {
    return internal::getTOMLValBase(t, keyName, storage, isOptional);
}

template <>
std::optional<std::shared_ptr<TOMLError>> getTOMLVal(toml::table&       t,
                                                     const std::string& keyName,
                                                     std::string&       storage,
                                                     const bool         isOptional);

template <>
std::optional<std::shared_ptr<TOMLError>> getTOMLVal(toml::table&              t,
                                                     const std::string&        keyName,
                                                     std::vector<std::string>& storage,
                                                     const bool                isOptional);
}  // namespace hatter