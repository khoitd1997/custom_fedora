#include "error_report_section_type.hpp"

#include <algorithm>

#include "formatter.hpp"
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

    std::transform(errors.begin(), errors.end(), std::back_inserter(ret), [&](auto error) {
        return formatter::formatText(sectionName, sectionFormat) + kErrorDelimiter + error->what();
    });

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

        std::transform(
            subReportStr.begin(), subReportStr.end(), std::back_inserter(ret), [&](auto subError) {
                return formatter::formatText(sectionName, sectionFormat) + kErrorLocationDelimiter +
                       subError;
            });
    }

    return ret;
}
TopSectionErrorReport::operator bool() const {
    return SubSectionErrorReport::operator bool() || (!errorReports.empty());
}

void processError(TopSectionErrorReport& errorReport, const SubSectionErrorReport& error) {
    if (error) { errorReport.errorReports.push_back(error); }
}
void processError(TopSectionErrorReport&                    errorReport,
                  const std::vector<SubSectionErrorReport>& errors) {
    for (const auto& error : errors) { processError(errorReport, error); }
}
}  // namespace hatter