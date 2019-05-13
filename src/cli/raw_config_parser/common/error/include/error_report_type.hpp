#pragma once

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "error_type.hpp"
namespace hatter {
struct ErrorReport {
   public:
    virtual ~ErrorReport() {}

    virtual void what() const = 0;
};

struct SubSectionErrorReport : public ErrorReport {
    // std::vector<std::shared_ptr<TOMLError>>         tomlErrors;
    // std::vector<std::shared_ptr<HatterParserError>> sanitizerErrors;
    std::vector<std::shared_ptr<HatterParserError>> errors;

    const std::string sectionName;

    explicit SubSectionErrorReport(const std::string& sectionName);

    virtual void what() const override;
};

struct TopSectionErrorReport : public SubSectionErrorReport {
    // std::vector<SubSectionErrorReport> subSectionErrors;
    std::vector<SubSectionErrorReport> errorReports;

    explicit TopSectionErrorReport(const std::string& sectionName);

    void what() const override;
};

struct SectionMergeErrorReport : public ErrorReport {
    const std::string                      sectionName;
    std::vector<SectionMergeConflictError> errors;

    void what() const override;

    explicit SectionMergeErrorReport(const std::string& sectionName);
};

struct FileErrorReport : public ErrorReport {
    std::vector<std::shared_ptr<ErrorReport>> errorReports;

    const std::string fileName;
    const std::string parentFile;

    FileErrorReport(const std::string& fileName, const std::string& parentFile);

    void what() const override;
};

template <typename T,
          typename V,
          std::enable_if_t<std::is_base_of<SubSectionErrorReport, T>::value>* = nullptr,
          std::enable_if_t<std::is_base_of<HatterParserError, V>::value>*     = nullptr>
bool processError(T& errorReport, const std::vector<std::shared_ptr<V>>&& errors) {
    errorReport.errors.insert(errorReport.errors.end(), errors.begin(), errors.end());
    if (!errors.empty()) { return true; }
    return false;
    // errorReport.sanitizerErrors.push_back(std::make_shared<V>(error));
}

template <typename T,
          typename V,
          std::enable_if_t<std::is_base_of<SubSectionErrorReport, T>::value>* = nullptr,
          std::enable_if_t<std::is_base_of<HatterParserError, V>::value>*     = nullptr>
bool processError(T& errorReport, const std::optional<std::shared_ptr<V>>&& error) {
    if (error) {
        errorReport.errors.push_back(*error);
        return true;
    }
    return false;
}

bool processError(TopSectionErrorReport&                      errorReport,
                  const std::optional<SubSectionErrorReport>& error);

bool processError(SectionMergeErrorReport&                        errorReport,
                  const std::optional<SectionMergeConflictError>& error);

bool processError(FileErrorReport&                            fileReport,
                  const std::optional<TopSectionErrorReport>& topReport);

bool processError(FileErrorReport&                              fileReport,
                  const std::optional<SectionMergeErrorReport>& mergeReport);

}  // namespace hatter