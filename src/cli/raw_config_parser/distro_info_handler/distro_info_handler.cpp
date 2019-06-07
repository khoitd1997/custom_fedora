#include "distro_info_handler.hpp"

#include <filesystem>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "toml11/toml.hpp"

#include "distro_info_get.hpp"
#include "distro_info_sanitize.hpp"

#include "raw_config_parser_utils.hpp"
#include "toml_utils.hpp"
#include "utils.hpp"

namespace hatter {
namespace distro_info_handler {
TopSectionErrorReport parse(toml::table& rawConfig, DistroInfo& outConf) {
    toml::table rawConf;
    bool        isEmpty;
    auto        errorReport = get(rawConfig, outConf, rawConf, isEmpty);
    if (!errorReport && !isEmpty) {
        errorReport.add(sanitize(outConf, rawConf));
    } else {
        std::cout << "get failed" << std::endl;
    }

    return errorReport;
}

TopSectionErrorReport merge(DistroInfo& resultConf, const DistroInfo& targetConf) {
    TopSectionErrorReport errorReport(resultConf.keyName);

    mergeAndCheckConflict(errorReport, resultConf.kickstartTag, targetConf.kickstartTag);
    mergeAndCheckConflict(errorReport, resultConf.baseSpin, targetConf.baseSpin);
    mergeAndCheckConflict(errorReport, resultConf.osName, targetConf.osName);

    return errorReport;
}
}  // namespace distro_info_handler
}  // namespace hatter