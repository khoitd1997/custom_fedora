#pragma once

#include <cxxabi.h>

#include <iostream>
#include <memory>
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
    std::cerr << "Trying to get name of unkown type " + std::string(rawName) << std::endl;
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
std::string getTypeName<hatter::CustomRepo>();

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
std::shared_ptr<TOMLError> getTOMLValBase(toml::table& t, ConfigMember<T>& conf) {
    TOMLValStatus status;
    auto          error = getTOMLValBase(t, conf, status);
    (void)status;
    return error;
}

template <typename T>
std::shared_ptr<TOMLError> getTOMLValBase(toml::table&     t,
                                          ConfigMember<T>& conf,
                                          TOMLValStatus&   status) {
    status = TOMLValStatus::PRESENT;
    try {
        conf.value = toml::get<T>(t.at(conf.keyName));
    } catch (const toml::type_error& e) {
        status = TOMLValStatus::PRESENT_TYPE_ERROR;
    } catch (const std::out_of_range& e) { status = TOMLValStatus::NOT_PRESENT; }
    t.erase(conf.keyName);

    switch (status) {
        case TOMLValStatus::NOT_PRESENT:
            if (conf.isOptional) { return nullptr; }
            return std::make_shared<TOMLExistentError>(conf.keyName);
            break;
        case TOMLValStatus::PRESENT:
            return nullptr;
            break;
        case TOMLValStatus::PRESENT_TYPE_ERROR:
            return std::make_shared<TOMLTypeError>(conf.keyName, getTypeName(conf.value));
            break;
        default:
            return nullptr;
            break;
    }
}

};  // namespace internal
template <typename T>
std::shared_ptr<TOMLError> getNonMemberTOMLVal(toml::table&       t,
                                               const std::string& keyName,
                                               T&                 out,
                                               const bool         isOptional = true) {
    ConfigMember<T> tempConf{keyName, .isOptional = isOptional};
    auto            error = internal::getTOMLValBase(t, tempConf);
    out                   = tempConf.value;
    return error;
}

template <typename T>
std::shared_ptr<TOMLError> getTOMLVal(toml::table& t, ConfigMember<T>& conf) {
    return internal::getTOMLValBase(t, conf);
}

template <>
std::shared_ptr<TOMLError> getTOMLVal(toml::table& t, ConfigMember<std::string>& conf);

std::shared_ptr<TOMLError> getBaseTable(toml::table&             t,
                                        const ConfigSectionBase& confSection,
                                        toml::table&             out);
std::shared_ptr<TOMLError> getBaseTable(toml::table&              t,
                                        const ConfigSectionBase&  confSection,
                                        std::vector<toml::table>& out);

template <>
std::shared_ptr<TOMLError> getTOMLVal(toml::table& t, ConfigMember<std::vector<std::string>>& conf);

std::shared_ptr<TOMLError> getTOMLVal(toml::table&                                      rawConf,
                                      ConfigMember<std::vector<std::filesystem::path>>& conf,
                                      const std::filesystem::path&                      parentDir);
}  // namespace hatter