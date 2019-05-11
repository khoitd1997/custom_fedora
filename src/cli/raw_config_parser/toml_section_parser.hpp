#pragma once

#include <map>
#include <memory>
#include <type_traits>
#include <utility>

#include "raw_config_parser.hpp"
#include "toml_utils.hpp"

namespace hatter {
namespace internal {
struct HatterParserLogicalError {
   protected:
    bool hasError_ = false;

   public:
    bool hasError() const;

    HatterParserLogicalError(const bool hasError);
    HatterParserLogicalError();
    virtual std::string what() const = 0;
    virtual ~HatterParserLogicalError();
};

struct UnknownValueError : public HatterParserLogicalError {
    std::vector<std::string> undefinedVals;

    std::string what() const override;

    explicit UnknownValueError(const toml::table& table);
};

struct RepoNoLinkError : public HatterParserLogicalError {
    std::string what() const override;

    explicit RepoNoLinkError(const Repo& repo);
};

struct RepoNoGPGKeyError : public HatterParserLogicalError {
    std::string what() const override;

    explicit RepoNoGPGKeyError(const Repo& repo);
};

struct ErrorReport {
   protected:
    bool hasError_ = false;

   public:
    virtual ~ErrorReport() {}

    bool hasError() const;
    void setError(const bool status);

    virtual void what() const = 0;
};

struct SubSectionErrorReport : public ErrorReport {
    std::vector<std::shared_ptr<TOMLError>>                tomlErrors;
    std::vector<std::shared_ptr<HatterParserLogicalError>> sanitizerErrors;

    const std::string sectionName;

    explicit SubSectionErrorReport(const std::string& sectionName);

    virtual void what() const override;
};

struct TopSectionErrorReport : public SubSectionErrorReport {
    std::vector<SubSectionErrorReport> subSectionErrors;

    explicit TopSectionErrorReport(const std::string& sectionName);

    void what() const override;
};

template <typename T,
          typename V,
          std::enable_if_t<std::is_base_of<SubSectionErrorReport, T>::value>*    = nullptr,
          std::enable_if_t<std::is_base_of<HatterParserLogicalError, V>::value>* = nullptr>
void processError(T& errorReport, const V&& error) {
    if (error.hasError()) {
        errorReport.setError(true);
        errorReport.sanitizerErrors.push_back(std::make_shared<V>(error));
    }
}

template <typename T,
          typename V,
          std::enable_if_t<std::is_base_of<SubSectionErrorReport, T>::value>* = nullptr,
          std::enable_if_t<std::is_base_of<TOMLError, V>::value>*             = nullptr>
void processError(T& errorReport, const std::shared_ptr<V>&& error) {
    if (error->hasError()) {
        errorReport.setError(true);
        errorReport.tomlErrors.push_back(error);
    }
}

void processError(TopSectionErrorReport& errorReport, const SubSectionErrorReport& error);

struct SectionMergeConflictError : public HatterParserLogicalError {
    const std::string keyName;
    const std::string val1;
    const std::string val2;

    SectionMergeConflictError(const std::string& keyName,
                              const std::string& val1,
                              const std::string& val2);
    bool hasError() const;

    std::string what() const override;
};

struct SectionMergeErrorReport : public ErrorReport {
    const std::string                      sectionName;
    std::vector<SectionMergeConflictError> errors;

    void what() const override;

    explicit SectionMergeErrorReport(const std::string& sectionName);
};

void processError(SectionMergeErrorReport& errorReport, const SectionMergeConflictError& error);
SectionMergeErrorReport merge(RepoConfig& resultConf, const RepoConfig& targetConf);

struct FileErrorReport : public ErrorReport {
    std::vector<TopSectionErrorReport>   sectionErrors;
    std::vector<SectionMergeErrorReport> mergeErrors;

    const std::string fileName;
    const std::string parentFile;

    FileErrorReport(const std::string& fileName, const std::string& parentFile);

    void what() const override;
};

TopSectionErrorReport getSection(toml::table& rawConfig, RepoConfig& repoConfig);

FileErrorReport getFile(const std::filesystem::path& filePath,
                        const std::string&           parentFileName,
                        FullConfig&                  fullConfig);
}  // namespace internal
}  // namespace hatter