#ifndef TOPOSORTER_H
#define TOPOSORTER_H

#include "graph/DGraphModel.h"
#include "list/DLinkedList.h"
#include "sorting/DLinkedListSE.h"

template <class T>
class TopoSorter {
   public:
    static int DFS;
    static int BFS;

   protected:
    DGraphModel<T> *graph;
    int (*hash_code)(T &, int);

   public:
    TopoSorter(DGraphModel<T> *graph, int (*hash_code)(T &, int) = 0) {
        this->graph = graph;
        this->hash_code = hash_code;
    }

    DLinkedList<T> sort(int mode = 0, bool sorted = true) {
        return (mode == BFS) ? bfsSort(sorted) : dfsSort(sorted);
    }

    DLinkedList<T> bfsSort(bool sorted = true) {
        DLinkedList<T> zeroInDegrees = listOfZeroInDegrees();
        if (sorted)
            mergeSort(zeroInDegrees);

        xMap<T, int> inDegree = vertex2inDegree(hash_code);

        Queue<T> q;
        for (int i = 0; i < zeroInDegrees.size(); i++)
            q.push(zeroInDegrees.get(i));

        DLinkedList<T> result;

        while (!q.empty()) {
            T current = q.pop();
            result.add(current);

            DLinkedList<T> nextNode = graph->getOutwardEdges(current);
            if (sorted)
                mergeSort(nextNode);

            typename DLinkedList<T>::Iterator it = nextNode.begin();
            for (it; it != nextNode.end(); it++) {
                T vertex = *it;
                int &degree = inDegree.get(vertex);
                degree--;

                if (degree == 0)
                    q.push(vertex);
            }
        }

        return result;
    }

    DLinkedList<T> dfsSort(bool sorted = true) {
        DLinkedList<T> vertices = graph->vertices();
        if (sorted)
            mergeSort(vertices);

        xMap<T, bool> visited(hash_code);
        for (int i = 0; i < vertices.size(); i++)
            visited.put(vertices.get(i), false);

        Stack<T> s;

        for (int i = 0; i < vertices.size(); i++) {
            T vertex = vertices.get(i);
            if (!visited.get(vertex))
                topoDFS(vertex, visited, s, sorted);
        }

        DLinkedList<T> result;

        while (!s.empty())
            result.add(s.pop());

        return result;
    }

   protected:
    xMap<T, int> vertex2inDegree(int (*hash)(T &, int));
    xMap<T, int> vertex2outDegree(int (*hash)(T &, int));
    DLinkedList<T> listOfZeroInDegrees();

    void mergeSort(DLinkedList<T> &list, int (*comparator)(T &, T &) = 0);
    void topoDFS(T vertex, xMap<T, bool> &visited, Stack<T> &s, bool sorted);
};

template <class T>
int TopoSorter<T>::DFS = 0;

template <class T>
int TopoSorter<T>::BFS = 1;

template <class T>
xMap<T, int> TopoSorter<T>::vertex2inDegree(int (*hash)(T &, int)) {
    xMap<T, int> inDegree(hash);

    typename DGraphModel<T>::Iterator it = graph->begin();
    for (it; it != graph->end(); it++) {
        T vertex = *it;
        inDegree.put(vertex, graph->inDegree(vertex));
    }

    return inDegree;
}

template <class T>
xMap<T, int> TopoSorter<T>::vertex2outDegree(int (*hash)(T &, int)) {
    xMap<T, int> outDegree(hash);

    typename DGraphModel<T>::Iterator it = graph->begin();
    for (it; it != graph->end(); it++) {
        T vertex = *it;
        outDegree.put(vertex, graph->outDegree(vertex));
    }

    return outDegree;
}

template <class T>
DLinkedList<T> TopoSorter<T>::listOfZeroInDegrees() {
    DLinkedList<T> zeroInDegrees;

    typename DGraphModel<T>::Iterator it = graph->begin();
    for (it; it != graph->end(); it++) {
        T vertex = *it;
        if (graph->inDegree(vertex) == 0)
            zeroInDegrees.add(vertex);
    }

    return zeroInDegrees;
}

template <class T>
void TopoSorter<T>::mergeSort(DLinkedList<T> &list, int (*comparator)(T &, T &)) {
    DLinkedListSE<T> sortedList(list);
    sortedList.sort(comparator);
    list = sortedList;
}

template <class T>
void TopoSorter<T>::topoDFS(T vertex, xMap<T, bool> &visited, Stack<T> &result, bool sorted) {
    Stack<T> s;
    s.push(vertex);

    while (!s.empty()) {
        T current = s.peek();

        bool &isVisited = visited.get(current);
        if (!isVisited)
            isVisited = true;

        bool visitAllNeighbours = true;
        DLinkedList<T> nextNode = graph->getOutwardEdges(current);
        if (sorted)
            mergeSort(nextNode);

        typename DLinkedList<T>::Iterator it = nextNode.begin();
        for (it; it != nextNode.end(); it++) {
            T nextNode = *it;
            if (!visited.get(nextNode)) {
                s.push(nextNode);
                visitAllNeighbours = false;
                break;
            }
        }

        if (visitAllNeighbours) {
            s.pop();
            result.push(current);
        }
    }
}

#endif  // TOPOSORTER_H
