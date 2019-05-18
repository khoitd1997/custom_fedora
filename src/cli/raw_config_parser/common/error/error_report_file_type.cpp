#include "error_report_file_type.hpp"

#include "ascii_code.hpp"
#include "utils.hpp"

namespace hatter {
FileSectionErrorReport::FileSectionErrorReport(const std::string& fileName,
                                               const std::string& parentFileName)
    : fileName{fileName}, parentFileName{parentFileName} {}
std::vector<std::string> FileSectionErrorReport::what() const {
    std::string includeStr =
        (parentFileName.empty()) ? "" : "(included from " + parentFileName + ")";
    const auto               fullFileName = formatStr(fileName, ascii_code::kBold) + includeStr;
    std::vector<std::string> ret;

    for (const auto& errorReport : errorReports) {
        for (const auto& error : errorReport.what()) { ret.push_back(fullFileName + "::" + error); }
    }

    return ret;
}
FileSectionErrorReport::operator bool() const { return (!errorReports.empty()); }

FileMergeErrorReport::FileMergeErrorReport(const std::string& firstFileName,
                                           const std::string& secondFileName)
    : firstFileName{firstFileName}, secondFileName{secondFileName} {}
std::vector<std::string> FileMergeErrorReport::what() const {
    std::vector<std::string> ret;

    for (const auto& errorReport : errorReports) {
        for (const auto& error : errorReport.what()) {
            ret.push_back(firstFileName + "<=>" + secondFileName + ":" + error);
        }
    }

    return ret;
}
FileMergeErrorReport::operator bool() const { return (!errorReports.empty()); }

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
FileErrorReport::operator bool() const {
    auto ret = false;
    try {
        ret = (std::get<FileSectionErrorReport>(errorReport)) ? true : false;
    } catch (const std::bad_variant_access&) {
        ret = (std::get<FileMergeErrorReport>(errorReport)) ? true : false;
    }
    return ret;
}

void processError(FileSectionErrorReport& fileReport, const TopSectionErrorReport& topReport) {
    if (topReport) { fileReport.errorReports.push_back(topReport); }
}

void processError(FileMergeErrorReport& fileReport, const SectionMergeErrorReport& mergeReport) {
    if (mergeReport) { fileReport.errorReports.push_back(mergeReport); }
}
}  // namespace hatter