#ifndef DGRAPHMODEL_H
#define DGRAPHMODEL_H

#include "graph/AbstractGraph.h"
#include "hash/xMap.h"
#include "sorting/DLinkedListSE.h"
#include "stacknqueue/Queue.h"
#include "stacknqueue/Stack.h"

template <class T>
class DGraphModel : public AbstractGraph<T> {
   public:
    DGraphModel(bool (*vertexEQ)(T &, T &), string (*vertex2str)(T &))
        : AbstractGraph<T>(vertexEQ, vertex2str) {}

    void connect(T from, T to, float weight = 0) {
        typename AbstractGraph<T>::VertexNode *fromNode = this->getVertexNode(from);
        typename AbstractGraph<T>::VertexNode *toNode = this->getVertexNode(to);

        if (!fromNode)
            throw VertexNotFoundException(this->vertex2str(from));
        if (!toNode)
            throw VertexNotFoundException(this->vertex2str(to));

        fromNode->connect(toNode, weight);
    }

    void disconnect(T from, T to) {
        typename AbstractGraph<T>::VertexNode *fromNode = this->getVertexNode(from);
        typename AbstractGraph<T>::VertexNode *toNode = this->getVertexNode(to);

        if (!fromNode)
            throw VertexNotFoundException(this->vertex2str(from));
        if (!toNode)
            throw VertexNotFoundException(this->vertex2str(to));

        typename AbstractGraph<T>::Edge *edge = fromNode->getEdge(toNode);
        if (!edge)
            throw EdgeNotFoundException("E(" + this->vertex2str(from) + "," + this->vertex2str(to) +
                                        ")");

        fromNode->adList.removeItem(edge);
        delete edge;
        edge = nullptr;

        fromNode->outDegree_--;
        toNode->inDegree_--;
    }

    void remove(T vertex) {
        typename AbstractGraph<T>::VertexNode *node = this->getVertexNode(vertex);

        if (!node)
            throw VertexNotFoundException(this->vertex2str(vertex));

        typename DLinkedList<typename AbstractGraph<T>::VertexNode *>::Iterator nodeIt =
            this->nodeList.begin();
        for (nodeIt; nodeIt != this->nodeList.end(); nodeIt++) {
            typename AbstractGraph<T>::VertexNode *currentNode = *nodeIt;
            if (!currentNode->equals(node))
                currentNode->removeTo(node);
        }

        typename DLinkedList<typename AbstractGraph<T>::Edge *>::Iterator edgeIt =
            node->adList.begin();
        for (edgeIt; edgeIt != node->adList.end(); edgeIt++) {
            typename AbstractGraph<T>::Edge *currentEdge = *edgeIt;
            currentEdge->to->inDegree_--;
        }

        node->adList.clear();

        this->nodeList.removeItem(node);
        delete node;
        node = nullptr;
    }

    static DGraphModel<T> *create(T *vertices, int nvertices, Edge<T> *edges, int nedges,
                                  bool (*vertexEQ)(T &, T &), string (*vertex2str)(T &)) {
        DGraphModel<T> *graph = new DGraphModel<T>(vertexEQ, vertex2str);

        for (int i = 0; i < nvertices; i++)
            graph->add(vertices[i]);

        for (int i = 0; i < nedges; i++) {
            T fromNode = edges[i].from;
            T toNode = edges[i].to;

            graph->connect(fromNode, toNode, edges[i].weight);
        }

        return graph;
    }
};

#endif  // DGRAPHMODEL_H
