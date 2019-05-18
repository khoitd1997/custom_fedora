#include "error_report_section_type.hpp"

#include <algorithm>

#include "utils.hpp"

namespace hatter {
SectionErrorReportBase::SectionErrorReportBase(const std::string& sectionName,
                                               const std::string& sectionFormat)
    : sectionName{sectionName}, sectionFormat{sectionFormat} {}
SectionErrorReportBase::~SectionErrorReportBase() {}

SubSectionErrorReport::SubSectionErrorReport(const std::string& sectionName,
                                             const std::string& sectionFormat)
    : SectionErrorReportBase(sectionName, sectionFormat) {}
SubSectionErrorReport::~SubSectionErrorReport() {}
std::vector<std::string> SubSectionErrorReport::what() const {
    std::vector<std::string> ret;
    for (const auto& error : errors) {
        ret.push_back(formatStr(sectionName, sectionFormat) + ": " + error->what());
    }

    return ret;
}
SubSectionErrorReport::operator bool() const { return (!errors.empty()); }

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
TopSectionErrorReport::operator bool() const {
    return SubSectionErrorReport::operator bool() || (!errorReports.empty());
}

SectionMergeErrorReport::SectionMergeErrorReport(const std::string& sectionName,
                                                 const std::string& sectionFormat)
    : SectionErrorReportBase(sectionName, sectionFormat) {}
std::vector<std::string> SectionMergeErrorReport::what() const {
    std::vector<std::string> ret;

    for (const auto& mergeErr : errors) {
        ret.push_back(formatStr(sectionName, sectionFormat) + ": " + mergeErr.what());
    }

    return ret;
}
SectionMergeErrorReport::operator bool() const { return (!errors.empty()); }

void processError(TopSectionErrorReport& errorReport, const SubSectionErrorReport& error) {
    if (error) { errorReport.errorReports.push_back(error); }
}

void processError(SectionMergeErrorReport& errorReport, const SectionMergeConflictError& error) {
    errorReport.errors.push_back(error);
}
}  // namespace hatter