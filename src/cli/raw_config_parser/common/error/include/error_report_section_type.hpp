#pragma once

#include <memory>
#include <string>
#include <vector>

#include "ascii_format.hpp"
#include "error_report_base_type.hpp"
#include "error_type.hpp"

namespace hatter {
static const auto kErrorDelimiter         = "||";
static const auto kErrorLocationDelimiter = "::";
struct SubSectionErrorReport : public ErrorReportBase {
    const std::string                               sectionName;
    const std::string                               sectionFormat;
    std::vector<std::shared_ptr<HatterParserError>> errors;

    SubSectionErrorReport(const std::string& sectionName,
                          const std::string& sectionFormat = formatter::kErrorSubSectionFormat);
    virtual ~SubSectionErrorReport();

    virtual explicit                 operator bool() const override;
    virtual std::vector<std::string> what() const override;

    void add(const std::vector<std::shared_ptr<HatterParserError>>&& parserErrors);
    // void add(const std::shared_ptr<HatterParserError>&& parserError);
};

struct TopSectionErrorReport : public SubSectionErrorReport {
    std::vector<SubSectionErrorReport> errorReports;

    TopSectionErrorReport(const std::string& sectionName,
                          const std::string& sectionFormat = formatter::kErrorTopSectionFormat);

    explicit                 operator bool() const override;
    std::vector<std::string> what() const override;

    using SubSectionErrorReport::add;
    void add(const std::vector<SubSectionErrorReport>& subReports);
    // void add(const SubSectionErrorReport& subReport);
};
}  // namespace hatter