#include "unit_test.hpp"

map<string, bool (UNIT_TEST_List::*)()> UNIT_TEST_List::TESTS;

// Helpers
string INT_toString(int &x) { return to_string(x); }
bool INT_EQ(int &a, int &b) { return a == b; }

bool newINT_EQ(int *&a, int *&b) { return *a == *b; }
string newINT_toString(int *&x) { return to_string(*x); }

void newINT_delete(XArrayList<int *> *list) {
    for (auto it = list->begin(); it != list->end(); it++)
        delete *it;
}

// Test cases
bool UNIT_TEST_List::list01() {
    string name = "list01";

    XArrayList<int> list;

    string expect = "[]";
    stringstream output;
    output << list.toString(INT_toString);

    return printResult(output.str(), expect, name);
}

bool UNIT_TEST_List::list02() {
    string name = "list02";

    XArrayList<int> list;
    list.add(1);
    list.add(2);
    list.add(3);

    string expect = "[1, 2, 3]";
    stringstream output;
    output << list.toString(INT_toString);

    return printResult(output.str(), expect, name);
}

bool UNIT_TEST_List::list03() {
    string name = "list03";

    XArrayList<int> list;
    list.add(1);
    list.add(3);
    list.add(1, 2);

    string expect = "[1, 2, 3]";
    stringstream output;
    output << list.toString(INT_toString);

    return printResult(output.str(), expect, name);
}

bool UNIT_TEST_List::list04() {
    string name = "list04";

    XArrayList<int> list;
    list.add(1);
    list.add(2);
    list.add(3);
    list.removeAt(1);

    string expect = "[1, 3]";
    stringstream output;
    output << list.toString(INT_toString);

    return printResult(output.str(), expect, name);
}

bool UNIT_TEST_List::list05() {
    string name = "list05";

    XArrayList<int> list(nullptr, INT_EQ);
    list.add(4);
    list.add(5);
    list.add(6);
    list.removeItem(5);

    string expect = "[4, 6]";
    stringstream output;
    output << list.toString(INT_toString);

    return printResult(output.str(), expect, name);
}

bool UNIT_TEST_List::list06() {
    string name = "list06";

    XArrayList<int> list;
    list.add(7);
    list.add(8);

    XArrayList<int> copy(list);

    string expect = "[7, 8]";
    stringstream output;
    output << copy.toString(INT_toString);

    return printResult(output.str(), expect, name);
}

bool UNIT_TEST_List::list07() {
    string name = "list07";

    XArrayList<int> list;
    list.add(9);
    list.add(10);

    XArrayList<int> copy;
    copy = list;

    string expect = "[9, 10]";
    stringstream output;
    output << copy.toString(INT_toString);

    return printResult(output.str(), expect, name);
}

bool UNIT_TEST_List::list08() {
    string name = "list08";

    XArrayList<int> list(nullptr, nullptr, 2);
    list.add(1);
    list.add(2);
    list.add(3);

    string expect = "[1, 2, 3]";
    stringstream output;
    output << list.toString(INT_toString);

    return printResult(output.str(), expect, name);
}

bool UNIT_TEST_List::list09() {
    string name = "list09";

    XArrayList<int *> list(newINT_delete, newINT_EQ);
    list.add(new int(1));
    list.add(new int(2));
    list.add(new int(3));

    string expect = "[1, 2, 3]";
    stringstream output;
    output << list.toString(newINT_toString);

    list.clear();
    return printResult(output.str(), expect, name);
}

bool UNIT_TEST_List::list10() {
    string name = "list10";

    XArrayList<int> list;
    list.add(1);
    list.add(2);
    list.add(3);

    stringstream output;
    output << "[";
    for (auto it = list.begin(); it != list.end(); it++) {
        output << *it;
        if (*it != 3)
            output << ", ";
    }
    output << "]";

    string expect = "[1, 2, 3]";
    return printResult(output.str(), expect, name);
}
