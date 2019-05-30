#pragma once

#include <string>
#include <variant>
#include <vector>

#include "error_report_base_type.hpp"
#include "error_report_section_type.hpp"

namespace hatter {
struct FileErrorReportBase : public ErrorReportBase {
    std::vector<TopSectionErrorReport> errorReports;

    void             add(const TopSectionErrorReport& errorReport);
    virtual explicit operator bool() const override;
};

struct FileSectionErrorReport : public FileErrorReportBase {
    const std::string fileName;
    const std::string parentFileName;

    FileSectionErrorReport(const std::string& fileName, const std::string& parentFileName);

    std::vector<std::string> what() const override;
};

struct FileMergeErrorReport : public FileErrorReportBase {
    const std::string firstFileName;
    const std::string secondFileName;

    FileMergeErrorReport(const std::string& firstFileName, const std::string& secondFileName);

    std::vector<std::string> what() const override;
};

struct FullErrorReport {
    std::vector<std::shared_ptr<FileErrorReportBase>> errorReports;

    explicit operator bool() const;
    void     what() const;

    void add(const std::shared_ptr<FileErrorReportBase>& errorReport);
};
}  // namespace hatter