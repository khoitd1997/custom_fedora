#include "error_report_file_type.hpp"

#include <algorithm>
#include <iomanip>
#include <sstream>

#include "ascii_code.hpp"
#include "utils.hpp"

namespace hatter {
namespace {
class PrettyPrinter {
   private:
    std::vector<std::string> errorLocations_;
    std::vector<std::string> errorMessages_;
    size_t                   maxLeft_ = 0;
    const size_t             kGapSize = 3;

   public:
    void addError(std::string& fullMessage) {
        const auto errorPart = strSplit(fullMessage, kErrorDelimiter, 1);

        maxLeft_ = std::max(maxLeft_, errorPart.at(0).length());
        errorLocations_.push_back(errorPart.at(0));

        errorMessages_.push_back(errorPart.at(1));
    }

    std::vector<std::string> makePrettyErrors() {
        std::vector<std::string> ret;
        for (size_t i = 0; i < errorLocations_.size(); ++i) {
            std::stringstream buffer;
            buffer << std::left << std::setfill(' ')
                   << std::setw(static_cast<int>(maxLeft_ + kGapSize)) << errorLocations_.at(i)
                   << errorMessages_.at(i);
            ret.push_back(buffer.str());
        }

        return ret;
    }
};
}  // namespace
FileSectionErrorReport::FileSectionErrorReport(const std::string& fileName,
                                               const std::string& parentFileName)
    : fileName{fileName}, parentFileName{parentFileName} {}
std::vector<std::string> FileSectionErrorReport::what() const {
    std::string includeStr =
        (parentFileName.empty()) ? "" : "(included from " + parentFileName + ")";
    const auto fullFileName = formatStr(fileName, ascii_code::kItalic) + includeStr;

    PrettyPrinter printer;

    for (const auto& errorReport : errorReports) {
        for (const auto& error : errorReport.what()) {
            auto fullMessage = fullFileName + kErrorLocationDelimiter + error;

            printer.addError(fullMessage);
        }
    }

    return printer.makePrettyErrors();
}
FileSectionErrorReport::operator bool() const { return (!errorReports.empty()); }

FileMergeErrorReport::FileMergeErrorReport(const std::string& firstFileName,
                                           const std::string& secondFileName)
    : firstFileName{firstFileName}, secondFileName{secondFileName} {}
std::vector<std::string> FileMergeErrorReport::what() const {
    PrettyPrinter printer;

    for (const auto& errorReport : errorReports) {
        for (const auto& error : errorReport.what()) {
            auto fullMessage =
                firstFileName + "<=>" + secondFileName + kErrorLocationDelimiter + error;

            printer.addError(fullMessage);
        }
    }

    return printer.makePrettyErrors();
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