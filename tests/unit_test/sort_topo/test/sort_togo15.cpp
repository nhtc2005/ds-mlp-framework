#include "../unit_test.hpp"

bool UNIT_TEST_Sort_Togo::sort_topo15() {
    string name = "sort_topo15";

    DGraphModel<char> model(&charComparator, &vertex2str);
    char vertices[] = {'F', 'G', 'B', 'C', 'D', 'E', 'A', 'X', 'Y', 'V'};
    for (int idx = 0; idx < 10; idx++) {
        model.add(vertices[idx]);
    }
    model.connect('F', 'Y');
    model.connect('D', 'X');
    model.connect('X', 'Y');
    model.connect('Y', 'V');
    TopoSorter<char> topoSorter(&model, &intKeyHash);
    DLinkedList<char> result = topoSorter.sort(TopoSorter<char>::DFS);

    string expect = "DFS Topological Sort: G->F->E->D->X->Y->V->C->B->A->NULL";

    stringstream output;
    output << "DFS Topological Sort: ";
    for (auto it = result.begin(); it != result.end(); it++) {
        output << *it << "->";
    }
    output << "NULL";

    model.clear();

    return printResult(output.str(), expect, name);
}
