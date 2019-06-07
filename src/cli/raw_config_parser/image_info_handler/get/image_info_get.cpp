#include "image_info_get.hpp"

#include "common_get.hpp"

namespace hatter {
namespace image_info_handler {
extern const std::string kSectionName;

TopSectionErrorReport get(toml::table&                 rawConfig,
                          const std::filesystem::path& fileDir,
                          ImageInfo&                   outConf,
                          toml::table&                 outRawConf) {
    TopSectionErrorReport errorReport(kSectionName);

    errorReport.add({getBaseTable(rawConfig, outConf, outRawConf)});
    if (errorReport || outRawConf.empty()) { return errorReport; }

    errorReport.add({getTOMLVal(outRawConf, outConf.partitionSize)});
    errorReport.add({getTOMLVal(outRawConf, outConf.firstLoginScripts, fileDir)});
    errorReport.add({getTOMLVal(outRawConf, outConf.postBuildScripts, fileDir)});
    errorReport.add({getTOMLVal(outRawConf, outConf.postBuildNoRootScripts, fileDir)});
    errorReport.add({getTOMLVal(outRawConf, outConf.userFiles, fileDir)});

    return errorReport;
}
}  // namespace image_info_handler
}  // namespace hatter