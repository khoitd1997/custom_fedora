/**
 * @file hatter_config_builder_parser.cpp
 * @author Khoi Trinh
 * @brief
 * @version 0.1
 * @date 2019-04-07
 *
 * @copyright Copyright Khoi Trinh (c) 2019
 *
 */

#include <cassert>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <string>

#include <execinfo.h>

#include "toml11/toml.hpp"

#include "build_variable.hpp"
#include "config_builder.hpp"
#include "config_decoder.hpp"
#include "logger.hpp"
#include "raw_config_parser.hpp"
#include "utils.hpp"

void crashHandler() {
    const size_t traceBufSize = 30;
    void*        traceElems[traceBufSize];
    const auto   traceCnt   = backtrace(traceElems, traceBufSize);
    auto         stackTrace = backtrace_symbols(traceElems, traceCnt);

    std::stringstream ss;
    for (auto i = 0; i < traceCnt; ++i) { ss << stackTrace[i] << std::endl; }
    hatter::logger::error(ss.str());

    free(stackTrace);
    exit(1);
}

int main() {
    hatter::logger::init();
    std::set_terminate(crashHandler);

    const hatter::build_variable::CLIBuildVariable currBuildVar;
    hatter::FullConfig                             currConfig;
    const auto                                     hasError =
        hatter::getFullConfig(hatter::build_variable::kParentConfigPath, currBuildVar, currConfig);

    // TODO(kd): Remove after test
    (void)(hasError);
    // if (hasError) {
    //     hatter::logger::error("failed to parse");
    //     return 1;
    // }

    if (!currBuildVar.parserMode) {
        hatter::FullConfig                       prevConfig;
        hatter::build_variable::CLIBuildVariable prevBuildVar;

        if (!hatter::build_variable::kIsFirstBuild) {
            prevBuildVar = hatter::build_variable::CLIBuildVariable{"prev"};
            hatter::getFullConfig(
                hatter::build_variable::kPrevParentConfigPath, currBuildVar, prevConfig, true);
            // TODO(kd): Enable after test
            //         assert(
            // !hatter::getFullConfig(hatter::build_variable::kPrevParentConfigPath, prevConfig));
        }
        hatter::config_builder::build(currConfig, prevConfig, currBuildVar, prevBuildVar);
    }

    hatter::writeFileWithHeader(hatter::config_decoder::decode(currConfig),
                                hatter::build_variable::kPrevParentConfigPath);

    return 0;
}
