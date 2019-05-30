#include "image_info_handler.hpp"

#include <filesystem>
#include <memory>
#include <string>
#include <vector>

#include "toml11/toml.hpp"

#include "image_info_get.hpp"
#include "image_info_sanitize.hpp"

#include "raw_config_parser_utils.hpp"
#include "toml_utils.hpp"
#include "utils.hpp"

namespace hatter {
namespace image_info_handler {
extern const std::string kSectionName = "image_info";

TopSectionErrorReport parse(toml::table&                 rawConfig,
                            const std::filesystem::path& fileDir,
                            ImageInfo&                   outConf) {
    toml::table rawImageConf;
    auto        errorReport = get(rawConfig, fileDir, outConf, rawImageConf);
    if (!errorReport) { errorReport.add(sanitize(outConf, rawImageConf)); }

    return errorReport;
}

TopSectionErrorReport merge(ImageInfo& resultConf, const ImageInfo& targetConf) {
    TopSectionErrorReport errorReport(kSectionName);
    (void)(resultConf);
    (void)(targetConf);

    // for (const auto& filePath : targetConf.mockScriptPaths) {
    //     auto isDup = false;
    //     for (const auto& resFilePath : resultConf.mockScriptPaths) {
    //         if (resFilePath.string() == filePath.string()) {
    //             isDup = true;
    //             break;
    //         }
    //     }
    //     if (!isDup) { resultConf.mockScriptPaths.push_back(filePath); }
    // }
    // mergeAndCheckConflict(errorReport,
    //                       "enable_custom_cache",
    //                       resultConf.enableCustomCache,
    //                       targetConf.enableCustomCache);

    return errorReport;
}
}  // namespace image_info_handler
}  // namespace hatter