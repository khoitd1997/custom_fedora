#include "common_get.hpp"

namespace hatter {
std::shared_ptr<TOMLError> getFilePath(toml::table&                        rawConf,
                                       const std::string&                  keyName,
                                       const std::filesystem::path&        fileDir,
                                       std::vector<std::filesystem::path>& out,
                                       const bool                          singleFile) {
    std::shared_ptr<TOMLError> error = nullptr;
    if (singleFile) {
        std::string tempStr;
        error = getTOMLVal(rawConf, keyName, tempStr);
        if (!error) { out.push_back(fileDir / tempStr); }
    } else {
        std::vector<std::string> tempStrs;
        error = getTOMLVal(rawConf, keyName, tempStrs);
        if (!error) { out.insert(out.end(), tempStrs.begin(), tempStrs.end()); }
    }
    return error;
}
}  // namespace hatter
