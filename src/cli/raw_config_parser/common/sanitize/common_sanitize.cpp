#include "common_sanitize.hpp"

#include "ascii_code.hpp"
#include "utils.hpp"

namespace hatter {
std::string UnknownValueError::what() const {
    auto undefinedStr = formatStr(strJoin(undefinedVals), ascii_code::kErrorListColor);
    return "unknown value(s): " + undefinedStr;
}

std::shared_ptr<UnknownValueError> checkUnknownValue(const toml::table& table) {
    if (!table.empty()) {
        auto error = std::make_shared<UnknownValueError>();
        for (auto const& [key, val] : table) { error->undefinedVals.push_back(key); }
        return error;
    }
    return nullptr;
}
}  // namespace hatter
