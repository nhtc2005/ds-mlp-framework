#ifndef ABSTRACTGRAPH_H
#define ABSTRACTGRAPH_H

#include <sstream>
#include <string>

#include "graph/IGraph.h"

using namespace std;

template <class T>
class AbstractGraph : public IGraph<T> {
   public:
    class Edge;
    class VertexNode;
    class Iterator;

   protected:
    DLinkedList<VertexNode *> nodeList;  // List of nodes

    bool (*vertexEQ)(T &, T &);
    string (*vertex2str)(T &);

    VertexNode *getVertexNode(T &vertex) {
        typename DLinkedList<VertexNode *>::Iterator it = nodeList.begin();
        while (it != nodeList.end()) {
            VertexNode *node = *it;
            if (vertexEQ(node->vertex, vertex))
                return node;
            it++;
        }
        return nullptr;
    }

    string vertex2Str(VertexNode &node) { return vertex2str(node.vertex); }
    string edge2Str(Edge &edge) {
        stringstream os;
        os << "E(" << vertex2str(edge.from->vertex) << "," << vertex2str(edge.to->vertex) << ")";
        return os.str();
    }

   public:
    AbstractGraph(bool (*vertexEQ)(T &, T &) = 0, string (*vertex2str)(T &) = 0) {
        this->vertexEQ = vertexEQ;
        this->vertex2str = vertex2str;
    }

    virtual ~AbstractGraph() {}

    typedef bool (*vertexEQFunc)(T &, T &);
    typedef string (*vertex2strFunc)(T &);
    vertexEQFunc getVertexEQ() { return this->vertexEQ; }
    vertex2strFunc getVertex2Str() { return this->vertex2str; }

    // Override in DGraphModel and UGraphModel
    virtual void connect(T from, T to, float weight = 0) = 0;
    virtual void disconnect(T from, T to) = 0;
    virtual void remove(T vertex) = 0;

    virtual void add(T vertex) {
        if (contains(vertex))
            return;
        VertexNode *newVertex = new VertexNode(vertex, vertexEQ, vertex2str);
        nodeList.add(newVertex);
    }

    virtual bool contains(T vertex) {
        typename DLinkedList<VertexNode *>::Iterator it = nodeList.begin();
        for (it; it != nodeList.end(); it++) {
            VertexNode *node = *it;
            if (vertexEQ(node->vertex, vertex))
                return true;
        }
        return false;
    }

    virtual float weight(T from, T to) {
        VertexNode *fromNode = getVertexNode(from);
        VertexNode *toNode = getVertexNode(to);

        if (!fromNode)
            throw VertexNotFoundException(vertex2str(from));
        if (!toNode)
            throw VertexNotFoundException(vertex2str(to));

        typename DLinkedList<Edge *>::Iterator it = fromNode->adList.begin();
        for (it; it != fromNode->adList.end(); it++) {
            Edge *edge = *it;
            if (vertexEQ(edge->to->vertex, to))
                return edge->weight;
        }

        throw EdgeNotFoundException("E(" + vertex2str(from) + "," + vertex2str(to) + ")");
    }

    virtual DLinkedList<T> getOutwardEdges(T from) {
        VertexNode *fromNode = getVertexNode(from);

        if (!fromNode)
            throw VertexNotFoundException(vertex2str(from));

        DLinkedList<T> outwardEdges = fromNode->getOutwardEdges();
        return outwardEdges;
    }

    virtual DLinkedList<T> getInwardEdges(T to) {
        VertexNode *toNode = getVertexNode(to);

        if (!toNode)
            throw VertexNotFoundException(vertex2str(to));

        DLinkedList<T> inwardEdges;

        typename DLinkedList<VertexNode *>::Iterator it = nodeList.begin();
        for (it; it != nodeList.end(); it++) {
            VertexNode *node = *it;
            if (node->getEdge(toNode))
                inwardEdges.add(node->vertex);
        }

        return inwardEdges;
    }

    virtual int size() { return nodeList.size(); }
    virtual bool empty() { return nodeList.empty(); }

    virtual void clear() {
        typename DLinkedList<VertexNode *>::Iterator it = nodeList.begin();
        for (it; it != nodeList.end(); it++) {
            VertexNode *node = *it;
            node->adList.clear();
            delete node;
        }
        nodeList.clear();
    }

    virtual int inDegree(T vertex) {
        VertexNode *node = getVertexNode(vertex);
        if (node)
            return node->inDegree_;
        throw VertexNotFoundException(vertex2str(vertex));
    }

    virtual int outDegree(T vertex) {
        VertexNode *node = getVertexNode(vertex);
        if (node)
            return node->outDegree_;
        throw VertexNotFoundException(vertex2str(vertex));
    }

    virtual DLinkedList<T> vertices() {
        DLinkedList<T> vertices;

        typename DLinkedList<VertexNode *>::Iterator it = nodeList.begin();
        for (it; it != nodeList.end(); it++) {
            VertexNode *node = *it;
            vertices.add(node->vertex);
        }

        return vertices;
    }

    virtual bool connected(T from, T to) {
        VertexNode *fromNode = getVertexNode(from);
        VertexNode *toNode = getVertexNode(to);

        if (!fromNode)
            throw VertexNotFoundException(vertex2str(from));
        if (!toNode)
            throw VertexNotFoundException(vertex2str(to));

        typename DLinkedList<Edge *>::Iterator it = fromNode->adList.begin();
        for (it; it != fromNode->adList.end(); it++) {
            Edge *edge = *it;
            if (vertexEQ(edge->to->vertex, to))
                return true;
        }

        return false;
    }

    void println() { cout << this->toString() << endl; }

    virtual string toString() {
        string mark(50, '=');
        stringstream os;

        os << mark << endl;
        os << left << setw(12) << "Vertices:" << endl;

        typename DLinkedList<VertexNode *>::Iterator nodeIt = nodeList.begin();
        while (nodeIt != nodeList.end()) {
            VertexNode *node = *nodeIt;
            os << node->toString() << endl;
            nodeIt++;
        }

        string mark2(30, '-');
        os << mark2 << endl;
        os << left << setw(12) << "Edges:" << endl;

        nodeIt = nodeList.begin();
        while (nodeIt != nodeList.end()) {
            VertexNode *node = *nodeIt;

            typename DLinkedList<Edge *>::Iterator edgeIt = node->adList.begin();
            while (edgeIt != node->adList.end()) {
                Edge *edge = *edgeIt;
                os << edge->toString() << endl;
                edgeIt++;
            }

            nodeIt++;
        }

        os << mark << endl;
        return os.str();
    }

    // begin, end: Use to traverse on graph's vertices
    Iterator begin() { return Iterator(this, true); }
    Iterator end() { return Iterator(this, false); }

   public:
    class VertexNode {
        friend class AbstractGraph;
        friend class Edge;

        template <class U>
        friend class UGraphModel;

        template <class D>
        friend class DGraphModel;

       private:
        T vertex;
        int inDegree_, outDegree_;
        DLinkedList<Edge *> adList;

        bool (*vertexEQ)(T &, T &);
        string (*vertex2str)(T &);

       public:
        VertexNode() : adList(&DLinkedList<Edge *>::free, &Edge::edgeEQ) {}
        VertexNode(T vertex, bool (*vertexEQ)(T &, T &), string (*vertex2str)(T &))
            : adList(&DLinkedList<Edge *>::free, &Edge::edgeEQ) {
            this->vertex = vertex;
            this->vertexEQ = vertexEQ;
            this->vertex2str = vertex2str;
            this->outDegree_ = this->inDegree_ = 0;
        }

        T &getVertex() { return vertex; }

        void connect(VertexNode *to, float weight = 0) {
            Edge *edge = getEdge(to);

            if (edge)
                edge->weight = weight;
            else {
                adList.add(new Edge(this, to, weight));
                this->outDegree_++;
                to->inDegree_++;
            }
        }

        DLinkedList<T> getOutwardEdges() {
            DLinkedList<T> edges;

            typename DLinkedList<Edge *>::Iterator it = adList.begin();
            for (it; it != adList.end(); it++) {
                Edge *edge = *it;
                edges.add(edge->to->vertex);
            }

            return edges;
        }

        Edge *getEdge(VertexNode *to) {
            typename DLinkedList<Edge *>::Iterator it = adList.begin();
            for (it; it != adList.end(); it++) {
                Edge *edge = *it;
                if (edge->to->equals(to))
                    return edge;
            }
            return nullptr;
        }

        bool equals(VertexNode *node) {
            if (vertexEQ)
                return vertexEQ(this->vertex, node->vertex);
            else
                return this->vertex == node->vertex;
        }

        void removeTo(VertexNode *to) {
            Edge *edge = getEdge(to);

            if (!edge)
                return;

            adList.removeItem(edge);
            delete edge;
            edge = nullptr;

            this->outDegree_--;
            to->inDegree_--;
        }

        int inDegree() { return inDegree_; }
        int outDegree() { return outDegree_; }

        string toString() {
            stringstream os;
            os << "V(" << this->vertex << ", " << "in: " << this->inDegree_ << ", "
               << "out: " << this->outDegree_ << ")";
            return os.str();
        }
    };

    class Edge {
        friend class VertexNode;
        friend class AbstractGraph;

        template <class U>
        friend class UGraphModel;

        template <class D>
        friend class DGraphModel;

       private:
        VertexNode *from;
        VertexNode *to;
        float weight;

       public:
        Edge() {}
        Edge(VertexNode *from, VertexNode *to, float weight = 0) {
            this->from = from;
            this->to = to;
            this->weight = weight;
        }

        bool equals(Edge *edge) {
            return this->from->equals(edge->from) && this->to->equals(edge->to) &&
                   this->weight == edge->weight;
        }

        static bool edgeEQ(Edge *&edge1, Edge *&edge2) { return edge1->equals(edge2); }

        string toString() {
            stringstream os;
            os << "E(" << this->from->vertex << "," << this->to->vertex << "," << this->weight
               << ")";
            return os.str();
        }
    };

    class Iterator {
       private:
        typename DLinkedList<VertexNode *>::Iterator nodeIt;

       public:
        Iterator(AbstractGraph<T> *pGraph = 0, bool begin = true) {
            if (begin) {
                if (pGraph != 0)
                    nodeIt = pGraph->nodeList.begin();
            } else if (begin == false) {
                if (pGraph != 0)
                    nodeIt = pGraph->nodeList.end();
            }
        }

        Iterator &operator=(const Iterator &iterator) {
            this->nodeIt = iterator.nodeIt;
            return *this;
        }

        T &operator*() { return (*nodeIt)->vertex; }

        bool operator!=(const Iterator &iterator) { return nodeIt != iterator.nodeIt; }

        // Prefix ++ overload
        Iterator &operator++() {
            nodeIt++;
            return *this;
        }

        // Postfix ++ overload
        Iterator operator++(int) {
            Iterator iterator = *this;
            ++*this;
            return iterator;
        }
    };
};

#endif  // ABSTRACTGRAPH_H
