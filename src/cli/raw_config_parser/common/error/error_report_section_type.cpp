#include "error_report_section_type.hpp"

#include <algorithm>

#include "formatter.hpp"
#include "utils.hpp"

namespace hatter {
SubSectionErrorReport::SubSectionErrorReport(const std::string& sectionName,
                                             const std::string& sectionFormat)
    : sectionName{sectionName}, sectionFormat{sectionFormat} {}
SubSectionErrorReport::~SubSectionErrorReport() {}
SubSectionErrorReport::operator bool() const {
    return (!errors.empty());
}
std::vector<std::string> SubSectionErrorReport::what() const {
    std::vector<std::string> ret;

    std::transform(errors.begin(), errors.end(), std::back_inserter(ret), [&](auto error) {
        return formatter::formatText(sectionName, sectionFormat) + kErrorDelimiter + error->what();
    });

    return ret;
}
void SubSectionErrorReport::add(
    const std::vector<std::shared_ptr<HatterParserError>>&& parserErrors) {
    std::copy_if(std::begin(parserErrors),
                 std::end(parserErrors),
                 std::back_inserter(errors),
                 [](const auto& error) { return error.operator bool(); });
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
void TopSectionErrorReport::add(const std::vector<SubSectionErrorReport>& subReports) {
    for (const auto& subReport : subReports) {
        if (subReport) { errorReports.push_back(subReport); }
    }
}
}  // namespace hatter