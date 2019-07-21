#include "image_info_get.hpp"

#include "common_get.hpp"

namespace hatter {
namespace image_info_handler {
TopSectionErrorReport get(toml::table&                 rawConfig,
                          const std::filesystem::path& fileDir,
                          ImageInfo&                   outConf,
                          toml::table&                 outRawConf,
                          bool&                        isEmpty) {
    TopSectionErrorReport errorReport("image_info");

    errorReport.add({getBaseTable(rawConfig, outConf, outRawConf)});
    isEmpty = outRawConf.empty();
    if (errorReport || outRawConf.empty()) { return errorReport; }

    errorReport.add({getTOMLVal(outRawConf, outConf.partitionSize),
                     getTOMLVal(outRawConf, outConf.firstLoginScripts, fileDir),
                     getTOMLVal(outRawConf, outConf.postBuildScripts, fileDir),
                     getTOMLVal(outRawConf, outConf.postBuildNoRootScripts, fileDir),
                     getTOMLVal(outRawConf, outConf.userFiles, fileDir)});

    return errorReport;
}
}  // namespace image_info_handler
}  // namespace hatter