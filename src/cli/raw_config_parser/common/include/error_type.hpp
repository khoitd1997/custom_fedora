#pragma once

#include <string>

struct HatterParserError {
    virtual std::string what() const = 0;
    virtual ~HatterParserError();
};

struct SectionMergeConflictError : public HatterParserError {
    const std::string keyName;
    const std::string val1;
    const std::string val2;

    SectionMergeConflictError(const std::string& keyName,
                              const std::string& val1,
                              const std::string& val2);

    std::string what() const override;
};
