#pragma once

#include <string>
#include <variant>
#include <vector>

#include "error_report_base_type.hpp"
#include "error_report_section_type.hpp"

namespace hatter {
struct FileSectionErrorReport : public ErrorReportBase {
    std::vector<TopSectionErrorReport> errorReports;
    const std::string                  fileName;
    const std::string                  parentFileName;

    FileSectionErrorReport(const std::string& fileName, const std::string& parentFileName);

    explicit                 operator bool() const override;
    std::vector<std::string> what() const override;
};

struct FileMergeErrorReport : public ErrorReportBase {
    std::vector<TopSectionErrorReport> errorReports;
    const std::string                  firstFileName;
    const std::string                  secondFileName;

    FileMergeErrorReport(const std::string& firstFileName, const std::string& secondFileName);

    explicit                 operator bool() const override;
    std::vector<std::string> what() const override;
};

struct FileErrorReport : public ErrorReportBase {
    const std::variant<FileSectionErrorReport, FileMergeErrorReport> errorReport;

    explicit FileErrorReport(const FileSectionErrorReport& sectionReport);
    explicit FileErrorReport(const FileMergeErrorReport& mergeReport);

    explicit                 operator bool() const override;
    std::vector<std::string> what() const override;
};

void processError(FileSectionErrorReport& fileReport, const TopSectionErrorReport& topReport);

void processError(FileMergeErrorReport& fileReport, const TopSectionErrorReport& mergeReport);
}  // namespace hatter