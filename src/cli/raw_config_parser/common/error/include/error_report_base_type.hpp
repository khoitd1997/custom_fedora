#pragma once

#include <string>
#include <vector>

namespace hatter {
struct ErrorReportBase {
   public:
    virtual ~ErrorReportBase() {}

    virtual std::vector<std::string> what() const = 0;
};
}  // namespace hatter