#include "error_report_type.hpp"

#include <iostream>
namespace hatter {
SubSectionErrorReport::SubSectionErrorReport(const std::string& sectionName)
    : sectionName{sectionName} {}
void SubSectionErrorReport::what() const {
    if (errors.size() > 0) {
        std::cout << sectionName << ":" << std::endl;

        for (const auto& error : errors) { std::cout << error->what() << std::endl; }
    }
}

TopSectionErrorReport::TopSectionErrorReport(const std::string& sectionName)
    : SubSectionErrorReport(sectionName) {}
void TopSectionErrorReport::what() const {
    std::cout << "top section " << sectionName << " error:" << std::endl;

    SubSectionErrorReport::what();
    for (const auto& sectionErr : errorReports) { sectionErr.what(); }
}

SectionMergeErrorReport::SectionMergeErrorReport(const std::string& sectionName)
    : sectionName(sectionName) {}
void SectionMergeErrorReport::what() const {
    std::cout << "merge error in section " << sectionName << ":" << std::endl;

    for (const auto& mergeErr : errors) { std::cout << mergeErr.what() << std::endl; }
}

FileErrorReport::FileErrorReport(const std::string& fileName, const std::string& parentFile)
    : fileName(fileName), parentFile(parentFile) {}
void FileErrorReport::what() const {
    if (errorReports.size() > 0) {
        std::string includeStr =
            (parentFile == "") ? " error:" : "(included from " + parentFile + "):";
        std::cout << fileName << includeStr << std::endl;

        for (const auto& errorReport : errorReports) { errorReport->what(); }
    }
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

bool processError(FileErrorReport&                            fileReport,
                  const std::optional<TopSectionErrorReport>& topReport) {
    if (topReport) {
        fileReport.errorReports.push_back(std::make_shared<TopSectionErrorReport>(*topReport));
        return true;
    }
    return false;
}

bool processError(FileErrorReport&                              fileReport,
                  const std::optional<SectionMergeErrorReport>& mergeReport) {
    if (mergeReport) {
        fileReport.errorReports.push_back(std::make_shared<SectionMergeErrorReport>(*mergeReport));
        return true;
    }
    return false;
}
}  // namespace hatter