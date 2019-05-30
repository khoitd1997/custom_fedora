#include "image_info_sanitize.hpp"

#include "common_sanitize.hpp"
#include "raw_config_parser_utils.hpp"

namespace hatter {
namespace image_info_handler {
std::vector<std::shared_ptr<HatterParserError>> sanitize(const ImageInfo&   imageConf,
                                                         const toml::table& table) {
    std::vector<std::shared_ptr<HatterParserError>> errors;

    if (auto error = checkUnknownValue(table)) { errors.push_back(error); }
    if (auto error = checkNotPositive("partition_size", imageConf.partitionSize)) {
        errors.push_back(error);
    }
    // if (auto error = checkFileNotExist(imageConf.)) { errors.push_back(error); }

    // if (auto error = checkFileNotExist(imageConf.mockScriptPaths.at(0))) {
    //     errors.push_back(error);
    // }

    return errors;
}
}  // namespace image_info_handler
}  // namespace hatter