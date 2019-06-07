#include "distro_info_get.hpp"

#include "common_get.hpp"

namespace hatter {
namespace distro_info_handler {
TopSectionErrorReport get(toml::table& rawConfig,
                          DistroInfo&  outConf,
                          toml::table& outRawConf,
                          bool&        isEmpty) {
    TopSectionErrorReport errorReport(outConf.keyName);

    errorReport.add({getBaseTable(rawConfig, outConf, outRawConf)});
    isEmpty = outRawConf.empty();
    if (errorReport || outRawConf.empty()) { return errorReport; }

    errorReport.add({getTOMLVal(outRawConf, outConf.kickstartTag),
                     getTOMLVal(outRawConf, outConf.baseSpin),
                     getTOMLVal(outRawConf, outConf.osName)});

    return errorReport;
}
}  // namespace distro_info_handler
}  // namespace hatter