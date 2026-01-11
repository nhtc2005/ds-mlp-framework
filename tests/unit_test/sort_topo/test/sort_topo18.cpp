#include "../unit_test.hpp"

bool UNIT_TEST_Sort_Togo::sort_topo18() {
    string name = "sort_topo18";

    DGraphModel<char> model(&charComparator, &vertex2str);
    TopoSorter<char> topoSorter(&model, &intKeyHash);
    DLinkedList<char> result = topoSorter.sort(TopoSorter<char>::BFS);

    string expect = "BFS Topological Sort: NULL";

    stringstream output;
    output << "BFS Topological Sort: ";
    for (auto it = result.begin(); it != result.end(); it++) {
        output << *it << "->";
    }
    output << "NULL";

    model.clear();

    return printResult(output.str(), expect, name);
}
