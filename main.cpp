#include "main.hpp"

#if defined(TEST_XARRAYLIST)
#include "unit_test/xarraylist/unit_test.hpp"
const string TEST_CASE = "XARRAYLIST";

#elif defined(TEST_DLINKEDLIST)
#include "unit_test/dlinkedlist/unit_test.hpp"
const string TEST_CASE = "DLINKEDLIST";

#elif defined(TEST_HASH)
#include "unit_test/hash/unit_test.hpp"
const string TEST_CASE = "HASH";

#elif defined(TEST_HEAP)
#include "unit_test/heap/unit_test.hpp"
const string TEST_CASE = "HEAP";

#elif defined(TEST_LAYER)
#include "unit_test/layer/unit_test.hpp"
const string TEST_CASE = "LAYER";

#elif defined(TEST_LOSS)
#include "unit_test/loss/unit_test.hpp"
const string TEST_CASE = "LOSS";

#elif defined(TEST_GRAPH)
#include "unit_test/graph/unit_test.hpp"
const string TEST_CASE = "GRAPH";

#elif defined(TEST_SORT_TOPO)
#include "unit_test/sort_topo/unit_test.hpp"
const string TEST_CASE = "SORT_TOPO";
#endif

void printTestCase();
void handleTestUnit(int argc, char* argv[]);

int main(int argc, char* argv[]) {
    if (argc <= 1) {
        printTestCase();
        return 1;
    }

    string arg1 = argv[1];

    if (arg1 == "test_unit") {
        cout << GREEN << BOLD << "Running unit_test/" << TEST_CASE << RESET << "\n";
        handleTestUnit(argc, argv);
    } else {
        printTestCase();
    }

    return 0;
}

#if defined(TEST_XARRAYLIST) || defined(TEST_DLINKEDLIST)
void handleTestUnit(int argc, char* argv[]) {
    UNIT_TEST_List unitTest;
    if (argc == 2 || (argc == 3 && string(argv[2]) == "all"))
        unitTest.runAllTests();
    else if (argc == 3)
        unitTest.runTest(argv[2]);
    else
        printTestCase();
}

#elif defined(TEST_HASH)
void handleTestUnit(int argc, char* argv[]) {
    UNIT_TEST_Hash unitTest;
    if (argc == 2 || (argc == 3 && string(argv[2]) == "all"))
        unitTest.runAllTests();
    else if (argc == 3)
        unitTest.runTest(argv[2]);
    else
        printTestCase();
}

#elif defined(TEST_HEAP)
void handleTestUnit(int argc, char* argv[]) {
    UNIT_TEST_Heap unitTest;
    if (argc == 2 || (argc == 3 && string(argv[2]) == "all"))
        unitTest.runAllTests();
    else if (argc == 3)
        unitTest.runTest(argv[2]);
    else
        printTestCase();
}

#elif defined(TEST_LAYER)
void handleTestUnit(int argc, char* argv[]) {
    UNIT_TEST_Layer unitTest;
    if (argc == 2 || (argc == 3 && string(argv[2]) == "all"))
        unitTest.runAllTests();
    else if (argc == 3)
        unitTest.runTest(argv[2]);
    else
        printTestCase();
}

#elif defined(TEST_LOSS)
void handleTestUnit(int argc, char* argv[]) {
    UNIT_TEST_Loss unitTest;
    if (argc == 2 || (argc == 3 && string(argv[2]) == "all"))
        unitTest.runAllTests();
    else if (argc == 3)
        unitTest.runTest(argv[2]);
    else
        printTestCase();
}

#elif defined(TEST_GRAPH)
void handleTestUnit(int argc, char* argv[]) {
    UNIT_TEST_Graph unitTest;
    if (argc == 2 || (argc == 3 && string(argv[2]) == "all"))
        unitTest.runAllTests();
    else if (argc == 3)
        unitTest.runTest(argv[2]);
    else
        printTestCase();
}

#elif defined(TEST_SORT_TOPO)
void handleTestUnit(int argc, char* argv[]) {
    UNIT_TEST_Sort_Togo unitTest;
    if (argc == 2 || (argc == 3 && string(argv[2]) == "all"))
        unitTest.runAllTests();
    else if (argc == 3)
        unitTest.runTest(argv[2]);
    else
        printTestCase();
}
#endif

void printTestCase() {
    std::cout << GREEN << BOLD << "Usage for running unit tests (" << TEST_CASE << "):" << RESET
              << "\n";
    std::cout << RED << "1. Run all tests:" << RESET << "\n";
    std::cout << "   ./main test_unit all\n\n";
    std::cout << RED << "2. Run a specific test function:" << RESET << "\n";
    std::cout << "   ./main test_unit <FunctionName>\n\n";
    std::cout << GREEN << "Example:" << RESET << "\n";
    std::cout << "   ./main test_unit test01\n";
}
