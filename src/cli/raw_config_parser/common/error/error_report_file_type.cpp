#include "error_report_file_type.hpp"

#include <algorithm>
#include <cctype>
#include <iomanip>
#include <iostream>
#include <sstream>

#include "ascii_format.hpp"
#include "formatter.hpp"
#include "logger.hpp"
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
                  3 * (std::count(errorLocation.begin(), errorLocation.end(), '\033') - 3))} {}
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
FileErrorReportBase::operator bool() const { return (!errorReports.empty()); }
void                 FileErrorReportBase::add(const std::vector<TopSectionErrorReport>& errorReps) {
    for (const auto& errorReport : errorReps) {
        if (errorReport) { errorReports.push_back(errorReport); }
    }
}

FileSectionErrorReport::FileSectionErrorReport(const std::string& fileName,
                                               const std::string& parentFileName)
    : fileName{fileName}, parentFileName{parentFileName} {}
std::vector<std::string> FileSectionErrorReport::what() const {
    const auto fullFileName = formatter::formatErrorFileLocation(fileName, parentFileName);

    PrettyPrinter printer;
    for (const auto& errorReport : errorReports) {
        for (const auto& error : errorReport.what()) {
            auto fullMessage = fullFileName + kErrorLocationDelimiter + error;

            printer.addError(fullMessage);
        }
    }

    return printer.makePrettyErrors();
}

FileMergeErrorReport::FileMergeErrorReport(const std::string& firstFileName,
                                           const std::string& secondFileName)
    : firstFileName{firstFileName}, secondFileName{secondFileName} {}
std::vector<std::string> FileMergeErrorReport::what() const {
    PrettyPrinter printer;

    for (const auto& errorReport : errorReports) {
        for (const auto& error : errorReport.what()) {
            auto fullMessage = formatter::formatErrorFileLocation(firstFileName) + "<=>" +
                               formatter::formatErrorFileLocation(secondFileName) +
                               kErrorLocationDelimiter + error;

            printer.addError(fullMessage);
        }
    }

    return printer.makePrettyErrors();
}

void FullErrorReport::what() const {
    for (const auto& errorReport : errorReports) {
        const auto errors = errorReport->what();
        for (const auto& error : errors) { logger::error(error); }
        logger::skipLine();
    }
}
FullErrorReport::operator bool() const { return (!errorReports.empty()); }
void             FullErrorReport::add(const std::shared_ptr<FileErrorReportBase>& errorReport) {
    if (errorReport) { errorReports.push_back(errorReport); }
}
}  // namespace hatter