#include "../unit_test.hpp"

bool UNIT_TEST_Sort_Togo::sort_topo08() {
    string name = "sort_topo08";

    DGraphModel<char> model(&charComparator, &vertex2str);
    char vertices[] = {'A', 'B', 'C', 'D', 'E'};
    for (int idx = 0; idx < 5; idx++) {
        model.add(vertices[idx]);
    }
    model.connect('A', 'B');
    model.connect('B', 'D');
    model.connect('B', 'C');
    model.connect('B', 'E');
    TopoSorter<char> topoSorter(&model, &intKeyHash);
    DLinkedList<char> result = topoSorter.sort(TopoSorter<char>::DFS);

    string expect = "DFS Topological Sort: A->B->E->D->C->NULL";

    stringstream output;
    output << "DFS Topological Sort: ";
    for (auto it = result.begin(); it != result.end(); it++) {
        output << *it << "->";
    }
    output << "NULL";

    model.clear();

    return printResult(output.str(), expect, name);
}
