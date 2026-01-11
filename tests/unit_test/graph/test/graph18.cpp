#include "../unit_test.hpp"

bool UNIT_TEST_Graph::graph18() {
    string name = "graph18";

    UGraphModel<char> model(&charComparator, &vertex2str);
    char vertices[] = {'A', 'B', 'C', 'D'};
    for (int idx = 0; idx < 4; idx++) {
        model.add(vertices[idx]);
    }

    model.connect('A', 'B');
    model.connect('B', 'A');

    string expect =
        "1 1\n\
==================================================\n\
Vertices:   \n\
V(A, in: 1, out: 1)\n\
V(B, in: 1, out: 1)\n\
V(C, in: 0, out: 0)\n\
V(D, in: 0, out: 0)\n\
------------------------------\n\
Edges:      \n\
E(A,B,0)\n\
E(B,A,0)\n\
==================================================\n";

    stringstream output;
    output << model.inDegree('A') << " " << model.outDegree('A') << endl;
    output << model.toString();

    model.clear();

    return printResult(output.str(), expect, name);
}
