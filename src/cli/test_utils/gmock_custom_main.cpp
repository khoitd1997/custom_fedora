#include <cassert>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string_view>
using namespace std::string_view_literals;

#include "gmock/gmock.h"

// got this from https://gist.github.com/elliotchance/8215283
class ConfigurableEventListener : public testing::TestEventListener {
   protected:
    testing::TestEventListener* eventListener;

   public:
    /**
     * Show the names of each test case.
     */

    bool showTestCases = false;
    /**
     * Show the iteration of each test case.
     */
    bool showTestIterations = false;

    /**
     * Show the names of each test.
     */
    bool showTestNames = false;

    /**
     * Show each success.
     */
    bool showSuccesses = false;

    /**
     * Show each failure as it occurs. You will also see it at the bottom after the full suite is
     * run.
     */
    bool showInlineFailures = true;

    /**
     * Show the setup of the global environment.
     */
    bool showEnvironment = false;

    explicit ConfigurableEventListener(TestEventListener* theEventListener)
        : eventListener(theEventListener) {}

    virtual ~ConfigurableEventListener() { delete eventListener; }

    virtual void OnTestProgramStart(const testing::UnitTest& unit_test) {
        eventListener->OnTestProgramStart(unit_test);
    }

    virtual void OnTestIterationStart(const testing::UnitTest& unit_test, int iteration) {
        if (showTestIterations) { eventListener->OnTestIterationStart(unit_test, iteration); }
    }

    virtual void OnEnvironmentsSetUpStart(const testing::UnitTest& unit_test) {
        if (showEnvironment) { eventListener->OnEnvironmentsSetUpStart(unit_test); }
    }

    virtual void OnEnvironmentsSetUpEnd(const testing::UnitTest& unit_test) {
        if (showEnvironment) { eventListener->OnEnvironmentsSetUpEnd(unit_test); }
    }

    virtual void OnTestCaseStart(const testing::TestCase& test_case) {
        if (showTestCases) { eventListener->OnTestCaseStart(test_case); }
    }

    virtual void OnTestStart(const testing::TestInfo& test_info) {
        if (showTestNames) { eventListener->OnTestStart(test_info); }
    }

    virtual void OnTestPartResult(const testing::TestPartResult& result) {
        (void)(result);
        // eventListener->OnTestPartResult(result);
    }

    virtual void OnTestEnd(const testing::TestInfo& test_info) {
        if ((showInlineFailures && test_info.result()->Failed()) ||
            (showSuccesses && !test_info.result()->Failed())) {
            eventListener->OnTestEnd(test_info);
        }
    }

    virtual void OnTestCaseEnd(const testing::TestCase& test_case) {
        if (showTestCases) { eventListener->OnTestCaseEnd(test_case); }
    }

    virtual void OnEnvironmentsTearDownStart(const testing::UnitTest& unit_test) {
        if (showEnvironment) { eventListener->OnEnvironmentsTearDownStart(unit_test); }
    }

    virtual void OnEnvironmentsTearDownEnd(const testing::UnitTest& unit_test) {
        if (showEnvironment) { eventListener->OnEnvironmentsTearDownEnd(unit_test); }
    }

    virtual void OnTestIterationEnd(const testing::UnitTest& unit_test, int iteration) {
        eventListener->OnTestIterationEnd(unit_test, iteration);
    }

    virtual void OnTestProgramEnd(const testing::UnitTest& unit_test) {
        eventListener->OnTestProgramEnd(unit_test);
    }
};

int main(int argc, char** argv) {
    auto envPtr = std::getenv("GTEST_FILTER");

    if (envPtr) { testing::GTEST_FLAG(filter) = envPtr; }
    // auto isDiscoverRun = false;
    // for (auto i = 0; i < argc; ++i) {
    //     if (argv[i] == "--gtest_list_tests"sv) {
    //         isDiscoverRun = true;
    //         break;
    //     }
    // }
    testing::InitGoogleMock(&argc, argv);

    // remove the default listener
    testing::TestEventListeners& listeners = testing::UnitTest::GetInstance()->listeners();
    auto default_printer                   = listeners.Release(listeners.default_result_printer());

    ConfigurableEventListener* listener = new ConfigurableEventListener(default_printer);
    listeners.Append(listener);

    // run
    // if (isDiscoverRun) { std::cout << "is discover run" << std::endl; }
    return RUN_ALL_TESTS();
}