#include "../unit_test.hpp"

bool UNIT_TEST_Graph::graph17() {
    string name = "graph17";

    UGraphModel<char> model(&charComparator, &vertex2str);

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
