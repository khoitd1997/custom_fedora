#pragma once

#include <memory>
#include <string>
#include <vector>

#include "ascii_code.hpp"
#include "error_report_base_type.hpp"
#include "error_type.hpp"

namespace hatter {
static const auto kErrorDelimiter         = "||";
static const auto kErrorLocationDelimiter = "::";

struct SectionErrorReportBase : public ErrorReportBase {
    const std::string sectionName;
    const std::string sectionFormat;

    SectionErrorReportBase(const std::string& sectionName, const std::string& sectionFormat);
    virtual ~SectionErrorReportBase();
};

struct SubSectionErrorReport : public SectionErrorReportBase {
    std::vector<std::shared_ptr<HatterParserError>> errors;

    SubSectionErrorReport(const std::string& sectionName,
                          const std::string& sectionFormat = ascii_code::kErrorSubSectionFormat);
    virtual ~SubSectionErrorReport();

    virtual explicit                 operator bool() const override;
    virtual std::vector<std::string> what() const override;
};

struct TopSectionErrorReport : public SubSectionErrorReport {
    std::vector<SubSectionErrorReport> errorReports;

    TopSectionErrorReport(const std::string& sectionName,
                          const std::string& sectionFormat = ascii_code::kErrorTopSectionFormat);

    explicit                 operator bool() const override;
    std::vector<std::string> what() const override;
};

template <typename T,
          typename V,
          std::enable_if_t<std::is_base_of<SubSectionErrorReport, T>::value>* = nullptr,
          std::enable_if_t<std::is_base_of<HatterParserError, V>::value>*     = nullptr>
void processError(T& errorReport, const std::vector<std::shared_ptr<V>>&& errors) {
    errorReport.errors.insert(errorReport.errors.end(), errors.begin(), errors.end());
}

template <typename T,
          typename V,
          std::enable_if_t<std::is_base_of<SubSectionErrorReport, T>::value>* = nullptr,
          std::enable_if_t<std::is_base_of<HatterParserError, V>::value>*     = nullptr>
void processError(T& errorReport, const std::shared_ptr<V>&& error) {
    if (error) { errorReport.errors.push_back(error); }
}

void processError(TopSectionErrorReport& errorReport, const SubSectionErrorReport& error);
void processError(TopSectionErrorReport&                    errorReport,
                  const std::vector<SubSectionErrorReport>& errors);
}  // namespace hatter