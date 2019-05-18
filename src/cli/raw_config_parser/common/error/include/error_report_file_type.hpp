#pragma once

#include <optional>
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

    std::vector<std::string> what() const override;
};

struct FileMergeErrorReport : public ErrorReportBase {
    std::vector<SectionMergeErrorReport> errorReports;
    const std::string                    firstFileName;
    const std::string                    secondFileName;

    FileMergeErrorReport(const std::string& firstFileName, const std::string& secondFileName);

    std::vector<std::string> what() const override;
};

struct FileErrorReport : public ErrorReportBase {
    const std::variant<FileSectionErrorReport, FileMergeErrorReport> errorReport;

    explicit FileErrorReport(const FileSectionErrorReport& sectionReport);
    explicit FileErrorReport(const FileMergeErrorReport& mergeReport);

    std::vector<std::string> what() const override;
};

bool processError(FileSectionErrorReport&                     fileReport,
                  const std::optional<TopSectionErrorReport>& topReport);

bool processError(FileMergeErrorReport&                         fileReport,
                  const std::optional<SectionMergeErrorReport>& mergeReport);
}  // namespace hatter