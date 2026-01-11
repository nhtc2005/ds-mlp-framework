#ifndef UNIT_TEST_LIST_HPP
#define UNIT_TEST_LIST_HPP

#include "library.hpp"
#include "list/DLinkedList.h"

class UNIT_TEST_List {
   public:
    UNIT_TEST_List() {
        registerTest("list01", &UNIT_TEST_List::list01);
        registerTest("list02", &UNIT_TEST_List::list02);
        registerTest("list03", &UNIT_TEST_List::list03);
        registerTest("list04", &UNIT_TEST_List::list04);
        registerTest("list05", &UNIT_TEST_List::list05);
        registerTest("list06", &UNIT_TEST_List::list06);
        registerTest("list07", &UNIT_TEST_List::list07);
        registerTest("list08", &UNIT_TEST_List::list08);
        registerTest("list09", &UNIT_TEST_List::list09);
        registerTest("list10", &UNIT_TEST_List::list10);
    }

   private:
    bool list01();
    bool list02();
    bool list03();
    bool list04();
    bool list05();
    bool list06();
    bool list07();
    bool list08();
    bool list09();
    bool list10();

   public:
    static map<string, bool (UNIT_TEST_List::*)()> TESTS;

    const string green = "\033[32m";
    const string red = "\033[31m";
    const string cyan = "\033[36m";
    const string reset = "\033[0m";

    bool printResult(string output, string expect, string name) {
        if (expect == output) {
            cout << green << "test " + name + " --------------- PASS" << reset << "\n";
            return true;
        } else {
            cout << red << "test " + name + " --------------- FAIL" << reset << "\n";
            cout << "\texpect : " << expect << endl;
            cout << "\toutput : " << output << endl;
            return false;
        }
    }

    // Run 1 test case
    void runTest(const std::string &name) {
        auto it = TESTS.find(name);
        if (it != TESTS.end()) {
            (this->*(it->second))();
        } else {
            throw std::runtime_error("Test with name '" + name + "' does not exist.");
        }
    }

    // Run all test cases
    void runAllTests() {
        vector<string> fails;
        for (const auto &test : TESTS) {
            if (!(this->*(test.second))()) {
                fails.push_back(test.first);
            }
        }

        cout << cyan << "\nResult -------------------------" << reset << endl;
        if (fails.empty()) {
            cout << green << "All tests passed!" << reset << endl;
        } else {
            int totalTests = TESTS.size();
            int failedTests = fails.size();
            int passedTests = totalTests - failedTests;
            double passRate =
                (totalTests > 0) ? (static_cast<double>(passedTests) / totalTests) * 100.0 : 0.0;
            cout << red << "Some tests failed:";
            for (const auto &fail : fails) {
                cout << "  " << fail;
            }
            cout << cyan << "\nPass rate: " << passRate << "%" << reset << endl;
        }
    }

    static void registerTest(string name, bool (UNIT_TEST_List::*function)()) {
        TESTS[name] = function;
    }
};

#endif  // UNIT_TEST_LIST_HPP
