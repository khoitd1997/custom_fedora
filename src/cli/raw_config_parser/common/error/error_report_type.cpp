#include "error_report_type.hpp"

#include <algorithm>
#include <iostream>

namespace hatter {
SubSectionErrorReport::SubSectionErrorReport(const std::string& sectionName)
    : sectionName{sectionName} {}
std::vector<std::string> SubSectionErrorReport::what() const {
    std::vector<std::string> ret;
    for (const auto& error : errors) { ret.push_back(sectionName + "::" + error->what()); }

    return ret;
}

TopSectionErrorReport::TopSectionErrorReport(const std::string& sectionName)
    : SubSectionErrorReport(sectionName) {}
std::vector<std::string> TopSectionErrorReport::what() const {
    std::vector<std::string> ret;

    auto report = SubSectionErrorReport::what();
    ret.insert(ret.end(), report.begin(), report.end());

    for (const auto& subErrorReport : errorReports) {
        auto subReportStr = subErrorReport.what();
        for (const auto& subError : subReportStr) { ret.push_back(sectionName + "::" + subError); }
    }

    return ret;
}

FileSectionErrorReport::FileSectionErrorReport(const std::string& fileName,
                                               const std::string& parentFileName)
    : fileName{fileName}, parentFileName{parentFileName} {}
std::vector<std::string> FileSectionErrorReport::what() const {
    std::string includeStr =
        (parentFileName.empty()) ? "" : "(included from " + parentFileName + ")";
    auto                     fullFileName = fileName + includeStr;
    std::vector<std::string> ret;

    for (const auto& errorReport : errorReports) {
        for (const auto& error : errorReport.what()) { ret.push_back(fullFileName + "::" + error); }
    }

    return ret;
}

SectionMergeErrorReport::SectionMergeErrorReport(const std::string& sectionName)
    : sectionName{sectionName} {}
std::vector<std::string> SectionMergeErrorReport::what() const {
    std::vector<std::string> ret;

    for (const auto& mergeErr : errors) { ret.push_back(sectionName + "::" + mergeErr.what()); }

    return ret;
}

FileMergeErrorReport::FileMergeErrorReport(const std::string& firstFileName,
                                           const std::string& secondFileName)
    : firstFileName{firstFileName}, secondFileName{secondFileName} {}
std::vector<std::string> FileMergeErrorReport::what() const {
    std::vector<std::string> ret;

    for (const auto& errorReport : errorReports) {
        for (const auto& error : errorReport.what()) {
            ret.push_back(firstFileName + "<=>" + secondFileName + "::" + error);
        }
    }

    return ret;
}

FileErrorReport::FileErrorReport(const FileSectionErrorReport& sectionReport)
    : errorReport{sectionReport} {}
FileErrorReport::FileErrorReport(const FileMergeErrorReport& mergeReport)
    : errorReport{mergeReport} {}
std::vector<std::string> FileErrorReport::what() const {
    std::vector<std::string> ret;

    try {
        ret = (std::get<FileSectionErrorReport>(errorReport)).what();
    } catch (const std::bad_variant_access&) {
        ret = (std::get<FileMergeErrorReport>(errorReport)).what();
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

bool processError(FileSectionErrorReport&                     fileReport,
                  const std::optional<TopSectionErrorReport>& topReport) {
    if (topReport) {
        fileReport.errorReports.push_back(*topReport);
        return true;
    }
    return false;
}

bool processError(FileMergeErrorReport&                         fileReport,
                  const std::optional<SectionMergeErrorReport>& mergeReport) {
    if (mergeReport) {
        fileReport.errorReports.push_back(*mergeReport);
        return true;
    }
    return false;
}
}  // namespace hatter