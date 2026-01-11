#include "../unit_test.hpp"

bool UNIT_TEST_Graph::graph14() {
    string name = "graph14";
    stringstream output;

    char vertices[] = {'A', 'B', 'C', 'D', 'E', 'F', 'H', 'G'};

    Edge<char> edges[6] = {Edge<char>('A', 'B', 1.5), Edge<char>('A', 'A', 10),
                           Edge<char>('B', 'C', 2.0), Edge<char>('B', 'B', 2.0),
                           Edge<char>('C', 'D', 3.2), Edge<char>('D', 'D', 3.2)};

    UGraphModel<char>* model =
        UGraphModel<char>::create(vertices, 8, edges, 6, &charComparator, &vertex2str);

    output << "Size: " << model->size() << endl;

    output << "vertices: ";
    DLinkedList<char> a = model->vertices();
    for (auto it = a.begin(); it != a.end(); ++it) {
        output << *it << " ";
    }
    output << "\n";

    output << "Indegree H: " << model->inDegree('H') << endl;
    output << "Outdegree C: " << model->outDegree('C') << endl;
    output << "connected AA : " << model->connected('A', 'A') << endl;
    output << "connected AH : " << model->connected('A', 'H') << endl;

    try {
        model->connected('A', 'X');
    } catch (const VertexNotFoundException& e) {
        output << "Error: Vertex X does not exist" << endl;
    }

    try {
        model->connected('V', 'Q');
    } catch (const VertexNotFoundException& e) {
        output << "Error: Vertex V does not exist" << endl;
    }

    output << "Size: " << model->size() << endl;

    string expect =
        "Size: 8\n\
vertices: A B C D E F H G \n\
Indegree H: 0\n\
Outdegree C: 2\n\
connected AA : 1\n\
connected AH : 0\n\
Error: Vertex X does not exist\n\
Error: Vertex V does not exist\n\
Size: 8\n\
==================================================\n\
Vertices:   \n\
V(A, in: 2, out: 2)\n\
V(B, in: 3, out: 3)\n\
V(C, in: 2, out: 2)\n\
V(D, in: 2, out: 2)\n\
V(E, in: 0, out: 0)\n\
V(F, in: 0, out: 0)\n\
V(H, in: 0, out: 0)\n\
V(G, in: 0, out: 0)\n\
------------------------------\n\
Edges:      \n\
E(A,B,1.5)\n\
E(A,A,10)\n\
E(B,A,1.5)\n\
E(B,C,2)\n\
E(B,B,2)\n\
E(C,B,2)\n\
E(C,D,3.2)\n\
E(D,C,3.2)\n\
E(D,D,3.2)\n\
==================================================\n";

    output << model->toString();

    model->clear();
    delete model;

    return printResult(output.str(), expect, name);
}
