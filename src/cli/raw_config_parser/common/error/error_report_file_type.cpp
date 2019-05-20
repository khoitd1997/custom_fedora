#include "error_report_file_type.hpp"

#include <algorithm>
#include <cctype>
#include <iomanip>
#include <iostream>
#include <sstream>

#include "ascii_format.hpp"
#include "utils.hpp"

namespace hatter {
namespace {
class PrettyPrinter {
   private:
    struct ErrorMessage {
        std::string errorLocation;
        std::string errorDetail;
        int         reduceLength;

        ErrorMessage(const std::string& errorLocation, const std::string& errorDetail)
            : errorLocation{errorLocation},
              errorDetail{errorDetail},
              reduceLength{static_cast<int>(
                  3 * (std::count(errorLocation.begin(), errorLocation.end(), '\033') - 1))} {}
    };

    std::vector<ErrorMessage> errorMessages;

    int       maxLeft_         = 0;
    int       maxReduceLength_ = 0;
    const int kGapSize         = 4;

   public:
    void addError(std::string& fullMessage) {
        const auto errorPart = strSplit(fullMessage, kErrorDelimiter, 1);

        errorMessages.push_back(ErrorMessage(errorPart.at(0), errorPart.at(1)));
        const auto currMessage = errorMessages.back();

        maxLeft_         = std::max(maxLeft_, static_cast<int>(currMessage.errorLocation.length()));
        maxReduceLength_ = std::max(maxReduceLength_, currMessage.reduceLength);
    }

    std::vector<std::string> makePrettyErrors() {
        std::vector<std::string> ret;

        std::sort(errorMessages.begin(), errorMessages.end(), [](ErrorMessage e1, ErrorMessage e2) {
            return e1.errorLocation.length() > e2.errorLocation.length();
        });

        for (const auto& errorMessage : errorMessages) {
            int width = 0;
            if (errorMessage.reduceLength != maxReduceLength_) {
                width = maxLeft_ + kGapSize - maxReduceLength_;
            } else {
                width = maxLeft_ + kGapSize;
            }
            std::stringstream buffer;
            buffer << std::left << std::setw(width) << errorMessage.errorLocation
                   << errorMessage.errorDetail;
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
    std::string includeStr   = (parentFileName.empty()) ? "" : "(" + parentFileName + ")";
    const auto  fullFileName = formatStr(fileName, ascii_format::kErrorFileLocationFormat) +
                              formatStr(includeStr, ascii_format::kFaint);

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
            auto fullMessage = formatStr(firstFileName, ascii_format::kErrorFileLocationFormat) +
                               "<=>" +
                               formatStr(secondFileName, ascii_format::kErrorFileLocationFormat) +
                               kErrorLocationDelimiter + error;

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

void processError(FileMergeErrorReport& fileReport, const TopSectionErrorReport& mergeReport) {
    if (mergeReport) { fileReport.errorReports.push_back(mergeReport); }
}
}  // namespace hatter