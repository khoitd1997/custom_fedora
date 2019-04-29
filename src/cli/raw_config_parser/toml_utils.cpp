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