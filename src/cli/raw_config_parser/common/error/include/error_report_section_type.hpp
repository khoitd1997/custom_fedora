#pragma once

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "error_report_base_type.hpp"
#include "error_type.hpp"

namespace hatter {
struct SectionErrorReport : public ErrorReportBase {
    const std::string sectionName;
    const std::string sectionFormatting;

    SectionErrorReport(const std::string& sectionName, const std::string& sectionFormatting);
    virtual ~SectionErrorReport();
};

struct SubSectionErrorReport : public SectionErrorReport {
    std::vector<std::shared_ptr<HatterParserError>> errors;

    SubSectionErrorReport(const std::string& sectionName,
                          const std::string& sectionFormatting = "");

    virtual std::vector<std::string> what() const override;
};

struct TopSectionErrorReport : public SubSectionErrorReport {
    std::vector<SubSectionErrorReport> errorReports;

    TopSectionErrorReport(const std::string& sectionName, const std::string& sectionFormatting);

    std::vector<std::string> what() const override;
};

struct SectionMergeErrorReport : public SectionErrorReport {
    std::vector<SectionMergeConflictError> errors;

    SectionMergeErrorReport(const std::string& sectionName, const std::string& sectionFormatting);

    std::vector<std::string> what() const override;
};

template <typename T,
          typename V,
          std::enable_if_t<std::is_base_of<SubSectionErrorReport, T>::value>* = nullptr,
          std::enable_if_t<std::is_base_of<HatterParserError, V>::value>*     = nullptr>
bool processError(T& errorReport, const std::vector<std::shared_ptr<V>>&& errors) {
    errorReport.errors.insert(errorReport.errors.end(), errors.begin(), errors.end());
    if (!errors.empty()) { return true; }
    return false;
}

template <typename T,
          typename V,
          std::enable_if_t<std::is_base_of<SubSectionErrorReport, T>::value>* = nullptr,
          std::enable_if_t<std::is_base_of<HatterParserError, V>::value>*     = nullptr>
bool processError(T& errorReport, const std::optional<std::shared_ptr<V>>&& error) {
    if (error) {
        errorReport.errors.push_back(*error);
        return true;
    }
    return false;
}

bool processError(TopSectionErrorReport&                      errorReport,
                  const std::optional<SubSectionErrorReport>& error);

bool processError(SectionMergeErrorReport&                        errorReport,
                  const std::optional<SectionMergeConflictError>& error);
}  // namespace hatter