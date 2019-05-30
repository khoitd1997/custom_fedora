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

    errorReport.add({getTOMLVal(rawConfig, kSectionName, outRawConf)});
    if (errorReport || outRawConf.empty()) { return errorReport; }

    errorReport.add({getTOMLVal(outRawConf, "partition_size", outConf.partitionSize)});
    errorReport.add(
        {getFilePath(outRawConf, "first_login_script", fileDir, outConf.firstLoginScripts)});
    errorReport.add(
        {getFilePath(outRawConf, "post_build_script", fileDir, outConf.postBuildScripts)});
    errorReport.add({getFilePath(
        outRawConf, "post_build_script_no_chroot", fileDir, outConf.postBuildNoRootScripts)});
    errorReport.add({getFilePath(outRawConf, "user_files", fileDir, outConf.userFiles, false)});

    return errorReport;
}
}  // namespace image_info_handler
}  // namespace hatter