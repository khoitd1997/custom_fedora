#pragma once

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
    auto totalError = 0;
    for (const auto& error : errors) {
        if (!(dynamic_cast<TErrorType*>(error.get()))) { ++totalError; }
    }

    return totalError;
}
}  // namespace hatter