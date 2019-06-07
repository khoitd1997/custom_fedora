#include "image_info_handler.hpp"

#include <algorithm>
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
    bool        isEmpty;
    auto        errorReport = get(rawConfig, fileDir, outConf, rawImageConf, isEmpty);
    if (!errorReport && !isEmpty) { errorReport.add(sanitize(outConf, rawImageConf)); }

    return errorReport;
}

TopSectionErrorReport merge(ImageInfo& resultConf, const ImageInfo& targetConf) {
    TopSectionErrorReport errorReport(kSectionName);

    resultConf.partitionSize.value =
        std::max(resultConf.partitionSize.value, targetConf.partitionSize.value);
    appendUniqueVector(resultConf.firstLoginScripts.value, targetConf.firstLoginScripts.value);
    appendUniqueVector(resultConf.postBuildScripts.value, targetConf.postBuildScripts.value);
    appendUniqueVector(resultConf.postBuildNoRootScripts.value,
                       targetConf.postBuildNoRootScripts.value);
    appendUniqueVector(resultConf.userFiles.value, targetConf.userFiles.value);

    return errorReport;
}
}  // namespace image_info_handler
}  // namespace hatter