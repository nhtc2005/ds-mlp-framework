#include "../unit_test.hpp"

bool UNIT_TEST_Graph::graph20() {
    string name = "graph20";

    UGraphModel<char> model(&charComparator, &vertex2str);
    char vertices[] = {'A', 'B', 'C', 'D'};
    for (int idx = 0; idx < 4; idx++) {
        model.add(vertices[idx]);
    }

    model.connect('A', 'B');
    model.disconnect('A', 'B');
    model.connect('A', 'B');
    model.disconnect('A', 'B');
    model.connect('A', 'B');
    model.disconnect('A', 'B');

    string expect =
        "==================================================\n\
Vertices:   \n\
V(A, in: 0, out: 0)\n\
V(B, in: 0, out: 0)\n\
V(C, in: 0, out: 0)\n\
V(D, in: 0, out: 0)\n\
------------------------------\n\
Edges:      \n\
==================================================\n";

    stringstream output;
    output << model.toString();

    model.clear();

    return printResult(output.str(), expect, name);
}
