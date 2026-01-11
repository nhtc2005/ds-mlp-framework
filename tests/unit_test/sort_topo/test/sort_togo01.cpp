#include "../unit_test.hpp"

bool UNIT_TEST_Sort_Togo::sort_topo01() {
    string name = "sort_topo01";

    Queue<char> queue;
    queue.push('A');
    queue.push('B');
    queue.push('C');

    string expect = "0 A B C D 1";

    stringstream output;
    output << queue.empty() << " " << queue.pop() << " ";
    queue.push('D');
    output << queue.pop() << " ";
    output << queue.pop() << " ";
    output << queue.pop() << " " << queue.empty();

    return printResult(output.str(), expect, name);
}
