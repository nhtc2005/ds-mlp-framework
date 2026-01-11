#include "../unit_test.hpp"

bool UNIT_TEST_Sort_Togo::sort_topo05() {
    string name = "sort_topo05";

    DLinkedListSE<int> data;
    data.add(1);
    data.add(2);
    data.add(0);
    data.add(3);
    data.add(3);
    data.add(8);
    data.add(9);
    data.sort();

    string expect =
        "[0, 1, 2, 3, 3, 8, 9]\n\
Sort: [9, 8, 3, 3, 2, 1, 0]";

    stringstream output;
    output << data.toString();
    output << "\nSort: [";
    for (auto it = data.bbegin(); it != data.bend(); it--) {
        if (it != data.bbegin())
            output << ", ";
        output << *it;
    }
    output << "]";

    data.clear();

    return printResult(output.str(), expect, name);
}
