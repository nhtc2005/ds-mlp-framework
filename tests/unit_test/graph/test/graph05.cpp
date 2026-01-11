#include "../unit_test.hpp"

bool UNIT_TEST_Graph::graph05() {
    string name = "graph05";
    stringstream output;

    char vertices[] = {'A', 'B', 'C', 'D'};

    Edge<char> edges[3] = {Edge<char>('A', 'B', 1.5), Edge<char>('B', 'C', 2.0),
                           Edge<char>('C', 'D', 3.2)};

    DGraphModel<char>* model =
        DGraphModel<char>::create(vertices, 4, edges, 3, &charComparator, &vertex2str);

    try {
        model->getOutwardEdges('E');
    } catch (const exception& e) {
        output << "Error: " << e.what() << endl;
    }

    DLinkedList<char> in = model->getInwardEdges('B');
    output << "getInwardEdges : B: ";
    for (auto it = in.begin(); it != in.end(); ++it) {
        output << *it << "-> ";
    }
    output << "NULL\n";

    string expect =
        "Error: Vertex (E): is not found\n\
getInwardEdges : B: A-> NULL\n\
==================================================\n\
Vertices:   \n\
V(A, in: 0, out: 1)\n\
V(B, in: 1, out: 1)\n\
V(C, in: 1, out: 1)\n\
V(D, in: 1, out: 0)\n\
------------------------------\n\
Edges:      \n\
E(A,B,1.5)\n\
E(B,C,2)\n\
E(C,D,3.2)\n\
==================================================\n";

    output << model->toString();

    model->clear();
    delete model;

    return printResult(output.str(), expect, name);
}
