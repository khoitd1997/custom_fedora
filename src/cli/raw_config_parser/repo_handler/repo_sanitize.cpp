#include "repo_sanitize.hpp"

#include "ascii_code.hpp"
#include "common_sanitize.hpp"
#include "logger.hpp"
#include "utils.hpp"

namespace hatter {
namespace repo_handler {
namespace {
const std::vector<std::string> supportedStandardRepos = {"google-chrome", "nvidia", "vscode"};
}

std::string StandardRepoNotSupportedError::what() const {
    const auto repoListStr = formatStr(strJoin(repos), ascii_code::kDarkYellow);

    return "the following " + formatStr("standard", ascii_code::kImportantWordFormat) +
           " repo(s) are not supported: " + repoListStr;
}
std::shared_ptr<StandardRepoNotSupportedError> checkStandardRepo(const RepoConfig& repoConf) {
    std::shared_ptr<StandardRepoNotSupportedError> error = nullptr;

    for (const auto& standardRepo : repoConf.standardRepos) {
        if (!inVector(standardRepo, supportedStandardRepos)) {
            if (!error) { error = std::make_shared<StandardRepoNotSupportedError>(); }
            error->repos.push_back(standardRepo);
        }
    }

    return error;
}

std::string CoprRepoNotFoundError::what() const {
    const auto repoListStr = formatStr(strJoin(repos), ascii_code::kDarkYellow);

    return "the following " + formatStr("copr", ascii_code::kImportantWordFormat) +
           " repo(s) can't be found: " + repoListStr;
}
std::shared_ptr<CoprRepoNotFoundError> checkCoprRepo(const RepoConfig& repoConf) {
    auto error    = std::make_shared<CoprRepoNotFoundError>();
    auto hasError = false;

    for (const auto& coprRepo : repoConf.coprRepos) {
        const auto temp = strSplit(coprRepo, "/");
        if (temp.size() != 2) {
            error->repos.push_back(coprRepo);
            hasError = true;
            continue;
        }
        const auto repoOwner = temp.at(0);

        // check if copr repo exists
        std::string dnfOutput;
        const auto  dnfCommand = "dnf copr list -q --available-by-user=" + repoOwner + " 2>&1";
        auto        errCode    = execCommand(dnfCommand, dnfOutput);
        if (errCode) {
            logger::error("dnf command \"" + dnfCommand + "\" failed with error: " + dnfOutput);
            exit(1);
        } else if (!inStr(coprRepo, dnfOutput)) {
            error->repos.push_back(coprRepo);
            hasError = true;
            continue;
        }
    }

    if (hasError) { return error; }
    return nullptr;
}

std::vector<std::shared_ptr<HatterParserError>> sanitize(const RepoConfig&  repoConf,
                                                         const toml::table& table) {
    std::vector<std::shared_ptr<HatterParserError>> errors;

    if (auto error = checkUnknownValue(table)) { errors.push_back(error); }
    if (auto error = checkStandardRepo(repoConf)) { errors.push_back(error); }
    if (auto error = checkCoprRepo(repoConf)) { errors.push_back(error); }

    return errors;
}
}  // namespace repo_handler
}  // namespace hatter