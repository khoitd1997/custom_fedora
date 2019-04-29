#ifndef _TOML_UTILS_HPP
#define _TOML_UTILS_HPP

#include <cxxabi.h>

#include <spdlog/spdlog.h>

#include <string>
#include <type_traits>
#include <typeinfo>
#include <vector>

#include "raw_config_parser.hpp"
#include "toml11/toml.hpp"

namespace internal {
template <typename T>
bool getTOMLBase(const toml::table& t,
                 const std::string& keyName,
                 T&                 storage,
                 const bool         isOptional,
                 const std::string& typeErrorMessage,
                 const std::string& undefinedErrorMessage) {
    try {
        storage = toml::get<T>(t.at(keyName));
    } catch (const toml::type_error& e) {
        spdlog::error(typeErrorMessage);
        return false;
    } catch (const std::out_of_range& e) {
        if (!isOptional) {
            spdlog::error(undefinedErrorMessage);
            return false;
        }
        return true;
    }
    return true;
}
}  // namespace internal

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

template <typename T>
bool getTOMLVal(const toml::table& t,
                const std::string& keyName,
                T&                 storage,
                const bool         isOptional = false) {
    return internal::getTOMLBase<T>(t,
                                    keyName,
                                    storage,
                                    isOptional,
                                    keyName + " needs to have type " + getTypeName(storage),
                                    keyName + "(" + getTypeName(storage) + ")" + " is undefined");
}

template <typename T>
bool getTOMLVal(const toml::table& t,
                const std::string& keyName,
                T&                 storage,
                const bool         isOptional,
                const std::string& typeErrorMessage,
                const std::string& undefinedErrorMessage) {
    return internal::getTOMLBase<T>(
        t, keyName, storage, isOptional, typeErrorMessage, undefinedErrorMessage);
}

#endif