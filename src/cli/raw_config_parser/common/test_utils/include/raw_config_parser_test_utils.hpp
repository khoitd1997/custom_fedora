#pragma once

#include <algorithm>
#include <functional>
#include <memory>
#include <vector>

#include "error_type.hpp"

namespace hatter {
template <typename TErrorType>
int checkTotalError(const std::vector<std::shared_ptr<HatterParserError>>& errors,
                    const std::function<void(TErrorType*)>&                callback = nullptr) {
    auto totalError = 0;
    for (const auto& error : errors) {
        if (auto errorPtr = dynamic_cast<TErrorType*>(error.get())) {
            ++totalError;
            if (callback) { callback(errorPtr); }
        }
    }

    return totalError;
}
template <typename TErrorType>
int checkTotalOtherError(const std::vector<std::shared_ptr<HatterParserError>>& errors) {
    return static_cast<int>(
        std::count_if(std::begin(errors), std::end(errors), [](const auto& error) {
            return !(dynamic_cast<TErrorType*>(error.get()));
        }));
}
}  // namespace hatter