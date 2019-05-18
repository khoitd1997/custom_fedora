#include "unknown_value_sanitize.hpp"

#include "utils.hpp"

namespace hatter {
std::string UnknownValueError::what() const {
    auto undefinedStr = strJoin(undefinedVals);
    return "unknown value(s): " + undefinedStr;
}

std::shared_ptr<UnknownValueError> checkUnknownValue(const toml::table& table) {
    if (!table.empty()) {
        auto error = std::make_shared<UnknownValueError>();
        std::cout << "Found unknown error" << std::endl;
        for (auto const& [key, val] : table) { error->undefinedVals.push_back(key); }
        return error;
    }
    return nullptr;
}
}  // namespace hatter
