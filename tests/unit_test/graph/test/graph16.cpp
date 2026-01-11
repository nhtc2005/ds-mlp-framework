#include "../unit_test.hpp"

bool UNIT_TEST_Graph::graph16() {
    string name = "graph16";

    DGraphModel<char> model(&charComparator, &vertex2str);

    string expect =
        "==================================================\n\
Vertices:   \n\
------------------------------\n\
Edges:      \n\
==================================================\n";

    stringstream output;
    output << model.toString();

    model.clear();

    return printResult(output.str(), expect, name);
}
