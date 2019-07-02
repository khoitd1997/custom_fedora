#pragma once

#include <string>

namespace hatter {
struct HatterParserError {
    virtual std::string what() const = 0;
    virtual ~HatterParserError();
};

struct SingleKeyError : public HatterParserError {
    const std::string keyName;

    explicit SingleKeyError(const std::string& keyName);
    virtual ~SingleKeyError();
};

struct SectionMergeConflictError : public SingleKeyError {
    const std::string val1;
    const std::string val2;

    SectionMergeConflictError(const std::string& keyName,
                              const std::string& val1,
                              const std::string& val2);

    std::string what() const override;
};
}  // namespace hatter