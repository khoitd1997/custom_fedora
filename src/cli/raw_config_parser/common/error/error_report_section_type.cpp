#include "error_report_section_type.hpp"

#include <algorithm>

#include "utils.hpp"

namespace hatter {
SectionErrorReport::SectionErrorReport(const std::string& sectionName,
                                       const std::string& sectionFormat)
    : sectionName{sectionName}, sectionFormat{sectionFormat} {}
SectionErrorReport::~SectionErrorReport() {}

SubSectionErrorReport::SubSectionErrorReport(const std::string& sectionName,
                                             const std::string& sectionFormat)
    : SectionErrorReport(sectionName, sectionFormat) {}
std::vector<std::string> SubSectionErrorReport::what() const {
    std::vector<std::string> ret;
    for (const auto& error : errors) {
        ret.push_back(formatStr(sectionName, sectionFormat) + ": " + error->what());
    }

    return ret;
}

TopSectionErrorReport::TopSectionErrorReport(const std::string& sectionName,
                                             const std::string& sectionFormat)
    : SubSectionErrorReport(sectionName, sectionFormat) {}
std::vector<std::string> TopSectionErrorReport::what() const {
    std::vector<std::string> ret;

    auto report = SubSectionErrorReport::what();
    ret.insert(ret.end(), report.begin(), report.end());

    for (const auto& subErrorReport : errorReports) {
        auto subReportStr = subErrorReport.what();
        for (const auto& subError : subReportStr) {
            ret.push_back(formatStr(sectionName, sectionFormat) + "::" + subError);
        }
    }

    return ret;
}

SectionMergeErrorReport::SectionMergeErrorReport(const std::string& sectionName,
                                                 const std::string& sectionFormat)
    : SectionErrorReport(sectionName, sectionFormat) {}
std::vector<std::string> SectionMergeErrorReport::what() const {
    std::vector<std::string> ret;

    for (const auto& mergeErr : errors) {
        ret.push_back(formatStr(sectionName, sectionFormat) + ": " + mergeErr.what());
    }

    return ret;
}

bool processError(TopSectionErrorReport&                      topReport,
                  const std::optional<SubSectionErrorReport>& subReport) {
    if (subReport) {
        topReport.errorReports.push_back(*subReport);
        return true;
    }
    return false;
}

bool processError(SectionMergeErrorReport&                        sectionReport,
                  const std::optional<SectionMergeConflictError>& error) {
    if (error) {
        sectionReport.errors.push_back(*error);
        return true;
    }
    return false;
}
}  // namespace hatter