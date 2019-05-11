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
    virtual std::string what() = 0;
    virtual ~HatterParserLogicalError();
};

struct UnknownValueError : public HatterParserLogicalError {
    std::vector<std::string> undefinedVals;

    std::string what() override;

    explicit UnknownValueError(const toml::table& table);
};

struct RepoNoLinkError : public HatterParserLogicalError {
    std::string what() override;

    explicit RepoNoLinkError(const Repo& repo);
};

struct RepoNoGPGKeyError : public HatterParserLogicalError {
    std::string what() override;

    explicit RepoNoGPGKeyError(const Repo& repo);
};

struct SubSectionErrorReport {
   protected:
    bool hasError_ = false;

   public:
    std::vector<std::shared_ptr<TOMLError>>                tomlErrors;
    std::vector<std::shared_ptr<HatterParserLogicalError>> sanitizerErrors;
    bool                                                   hasError() const;
    void                                                   setError(const bool status);

    const std::string sectionName;

    explicit SubSectionErrorReport(const std::string& sectionName);
};

struct TopSectionErrorReport : public SubSectionErrorReport {
    std::vector<SubSectionErrorReport> subSectionErrors;

    explicit TopSectionErrorReport(const std::string& sectionName);
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

struct SectionMergeErrorReport {
    const std::string                                      sectionName;
    std::vector<std::shared_ptr<HatterParserLogicalError>> errors;

    SectionMergeErrorReport(const std::string& sectionName);
};

// struct FileMergeErrorReport

TopSectionErrorReport getSection(const toml::table& rawConfig, RepoConfig& repoConfig);
}  // namespace internal
}  // namespace hatter