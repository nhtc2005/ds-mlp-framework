#ifndef DLINKEDLIST_H
#define DLINKEDLIST_H

#include <iostream>
#include <sstream>
#include <type_traits>

#include "list/IList.h"

using namespace std;

template <class T>
class DLinkedList : public IList<T> {
   public:
    class Node;
    class Iterator;
    class BWDIterator;

   protected:
    Node *head;  // This node does not contain user's data (Dummy node)
    Node *tail;  // This node does not contain user's data (Dummy node)
    int count;
    bool (*itemEqual)(T &lhs,
                      T &rhs);  // Function pointer: test if two items (type: T&) are equal or not
    void (*deleteUserData)(DLinkedList<T> *);  // Function pointer: be called to remove items (if
                                               // they are pointer type)

    void copyFrom(const DLinkedList<T> &list) {
        clear();

        // Deep copy
        Node *current = list.head->next;
        while (current != list.tail) {
            this->add(current->data);
            current = current->next;
        }

        this->deleteUserData = list.deleteUserData;
        this->itemEqual = list.itemEqual;
    }

   public:
    DLinkedList(void (*deleteUserData)(DLinkedList<T> *) = 0, bool (*itemEqual)(T &, T &) = 0);
    DLinkedList(const DLinkedList<T> &list);
    DLinkedList<T> &operator=(const DLinkedList<T> &list);
    ~DLinkedList();

    // Inherit from IList
    void add(T e);
    void add(int index, T e);
    T removeAt(int index);
    bool removeItem(T item, void (*removeItemData)(T) = 0);
    bool empty();
    int size();
    void clear();
    T &get(int index);
    int indexOf(T item);
    bool contains(T item);
    string toString(string (*item2str)(T &) = 0);

    void println(string (*item2str)(T &) = 0) { cout << toString(item2str) << endl; }
    void setDeleteUserDataPtr(void (*deleteUserData)(DLinkedList<T> *) = 0) {
        this->deleteUserData = deleteUserData;
    }

    bool contains(T array[], int size) {
        int idx = 0;
        for (DLinkedList<T>::Iterator it = begin(); it != end(); it++)
            if (!equals(*it, array[idx++], this->itemEqual))
                return false;
        return true;
    }

    /*
    free(DLinkedList<T> *list):
        To remove user's data (type T, must be a pointer type, e.g.: int*, Point*)
        if users want a DLinkedList removing their data, he/she must pass "free" to constructor of
    DLinkedList Example: DLinkedList<T> list(&DLinkedList<T>::free);
    */
    static void free(DLinkedList<T> *list) {
        typename DLinkedList<T>::Iterator it = list->begin();
        while (it != list->end()) {
            delete *it;
            it++;
        }
    }

    // begin, end and Iterator helps user to traverse a list forwardly
    Iterator begin() { return Iterator(this, true); }
    Iterator end() { return Iterator(this, false); }

    // bbegin, bend and BWDIterator helps user to traverse a list backwardly
    BWDIterator bbegin() { return BWDIterator(this, true); }
    BWDIterator bend() { return BWDIterator(this, false); }

   protected:
    static bool equals(T &lhs, T &rhs, bool (*itemEqual)(T &, T &)) {
        if (itemEqual == 0)
            return lhs == rhs;
        else
            return itemEqual(lhs, rhs);
    }

    Node *getNodeOf(int index);
    void checkIndex(int index);

   public:
    class Node {
       public:
        T data;
        Node *next;
        Node *prev;
        friend class DLinkedList<T>;

       public:
        Node(Node *next = 0, Node *prev = 0) {
            this->next = next;
            this->prev = prev;
        }

        Node(T data, Node *next = 0, Node *prev = 0) {
            this->data = data;
            this->next = next;
            this->prev = prev;
        }
    };

    class Iterator {
       private:
        DLinkedList<T> *pList;
        Node *pNode;

       public:
        Iterator(DLinkedList<T> *pList = 0, bool begin = true) {
            if (begin) {
                if (pList != 0)
                    this->pNode = pList->head->next;
                else
                    pNode = 0;
            } else {
                if (pList != 0)
                    this->pNode = pList->tail;
                else
                    pNode = 0;
            }
            this->pList = pList;
        }

        Iterator &operator=(const Iterator &iterator) {
            this->pNode = iterator.pNode;
            this->pList = iterator.pList;
            return *this;
        }

        void remove(void (*removeItemData)(T) = 0) {
            pNode->prev->next = pNode->next;
            pNode->next->prev = pNode->prev;
            Node *pNext = pNode->prev;  // Must prev, so iterator ++ will go to end
            if (removeItemData != 0)
                removeItemData(pNode->data);
            delete pNode;
            pNode = pNext;
            pList->count -= 1;
        }

        T &operator*() { return pNode->data; }
        bool operator!=(const Iterator &iterator) { return pNode != iterator.pNode; }

        // Prefix ++ overload
        Iterator &operator++() {
            pNode = pNode->next;
            return *this;
        }

        // Postfix ++ overload
        Iterator operator++(int) {
            Iterator iterator = *this;
            ++*this;
            return iterator;
        }
    };

    class BWDIterator {
       private:
        DLinkedList<T> *pList;
        Node *pNode;

       public:
        BWDIterator(DLinkedList<T> *pList = 0, bool begin = true) {
            if (begin) {
                if (pList != 0)
                    this->pNode = pList->tail->prev;
                else
                    pNode = 0;
            } else {
                if (pList != 0)
                    this->pNode = pList->head;
                else
                    pNode = 0;
            }
            this->pList = pList;
        }

        BWDIterator &operator=(const BWDIterator &iterator) {
            this->pNode = iterator.pNode;
            this->pList = iterator.pList;
            return *this;
        }

        void remove(void (*removeItemData)(T) = 0) {
            pNode->prev->next = pNode->next;
            pNode->next->prev = pNode->prev;
            Node *pPrev = pNode->next;  // Must next, so iterator -- will go to begin
            if (removeItemData != 0)
                removeItemData(pNode->data);
            delete pNode;
            pNode = pPrev;
            pList->count -= 1;
        }

        T &operator*() { return pNode->data; }
        bool operator!=(const BWDIterator &iterator) { return pNode != iterator.pNode; }

        // Prefix ++ overload
        BWDIterator &operator++() {
            pNode = pNode->prev;
            return *this;
        }

        // Postfix ++ overload
        BWDIterator operator++(int) {
            BWDIterator iterator = *this;
            ++*this;
            return iterator;
        }

        // Prefix -- overload
        BWDIterator &operator--() {
            pNode = pNode->prev;
            return *this;
        }

        // Postfix -- overload
        BWDIterator operator--(int) {
            BWDIterator iterator = *this;
            --*this;
            return iterator;
        }
    };
};

// Define a shorter name for DLinkedList:
template <class T>
using List = DLinkedList<T>;

template <class T>
DLinkedList<T>::DLinkedList(void (*deleteUserData)(DLinkedList<T> *), bool (*itemEqual)(T &, T &))
    : deleteUserData(deleteUserData), itemEqual(itemEqual), count(0) {
    head = new Node();  // Dummy node
    tail = new Node();  // Dummy node
    head->next = tail;
    tail->prev = head;
}

template <class T>
DLinkedList<T>::DLinkedList(const DLinkedList<T> &list) {
    // Create dummy node
    head = new Node();
    tail = new Node();
    head->next = this->tail;
    tail->prev = this->head;
    count = 0;

    // Deep copy
    Node *current = list.head->next;
    while (current != list.tail) {
        this->add(current->data);
        current = current->next;
    }

    this->deleteUserData = list.deleteUserData;
    this->itemEqual = list.itemEqual;
}

template <class T>
DLinkedList<T> &DLinkedList<T>::operator=(const DLinkedList<T> &list) {
    if (this == &list)  // Check for seft-assignment
        return *this;

    // Delete current data
    this->clear();

    // Deep copy
    Node *current = list.head->next;
    while (current != list.tail) {
        this->add(current->data);
        current = current->next;
    }

    this->deleteUserData = list.deleteUserData;
    this->itemEqual = list.itemEqual;
    return *this;
}

template <class T>
DLinkedList<T>::~DLinkedList() {
    clear();
    delete head;
    delete tail;
}

template <class T>
void DLinkedList<T>::add(T e) {
    Node *newNode = new Node(e, tail, tail->prev);
    tail->prev->next = newNode;
    tail->prev = newNode;
    count++;
}

template <class T>
void DLinkedList<T>::add(int index, T e) {
    if ((index < 0) || (index > count))
        throw std::out_of_range("Index is out of range!");
    Node *current = head;
    int current_index = -1;
    while (current_index != index) {
        current = current->next;
        current_index++;
    }
    Node *newNode = new Node(e, current, current->prev);
    current->prev->next = newNode;
    current->prev = newNode;
    count++;
}

template <class T>
void DLinkedList<T>::checkIndex(int index) {
    if ((index < 0) || (index >= count))
        throw std::out_of_range("Index is out of range!");
}

template <class T>
typename DLinkedList<T>::Node *DLinkedList<T>::getNodeOf(int index) {
    checkIndex(index);
    Node *current = nullptr;
    int current_index;
    if (index <= count / 2) {
        current = head;
        current_index = -1;
        while (current_index < index) {
            current = current->next;
            current_index++;
        }
    } else {
        current = tail;
        current_index = count;
        while (current_index > index) {
            current = current->prev;
            current_index--;
        }
    }
    return current;
}

template <class T>
T DLinkedList<T>::removeAt(int index) {
    Node *current = getNodeOf(index);
    T deletingData = current->data;
    current->prev->next = current->next;
    current->next->prev = current->prev;
    delete current;
    count--;
    return deletingData;
}

template <class T>
bool DLinkedList<T>::empty() {
    if (count == 0)
        return true;
    return false;
}

template <class T>
int DLinkedList<T>::size() {
    return count;
}

template <class T>
void DLinkedList<T>::clear() {
    if (deleteUserData != nullptr)
        deleteUserData(this);
    Node *current = head->next;
    while (current != tail) {
        Node *next = current->next;
        delete current;
        current = next;
    }
    head->next = tail;
    tail->prev = head;
    count = 0;
}

template <class T>
T &DLinkedList<T>::get(int index) {
    return getNodeOf(index)->data;
}

template <class T>
int DLinkedList<T>::indexOf(T item) {
    Node *current = head->next;
    int current_index = 0;
    while ((current_index < count) && (!equals(current->data, item, itemEqual))) {
        current = current->next;
        current_index++;
    }
    if (current_index < count)
        return current_index;
    return -1;
}

template <class T>
bool DLinkedList<T>::removeItem(T item, void (*removeItemData)(T)) {
    Node *current = head->next;
    for (int i = 0; i < count; i++) {
        if (equals(current->data, item, itemEqual)) {
            if (removeItemData != nullptr)
                removeItemData(current->data);
            removeAt(i);
            return true;
        }
        current = current->next;
    }
    return false;
}

template <class T>
bool DLinkedList<T>::contains(T item) {
    return indexOf(item) != -1;
}

template <class T>
string DLinkedList<T>::toString(string (*item2str)(T &)) {
    Node *current = head->next;
    ostringstream oss;
    oss << "[";
    for (int i = 0; i < count; i++) {
        if (item2str != nullptr)
            oss << item2str(current->data);
        else
            oss << current->data;
        if (i < count - 1)
            oss << ", ";
        current = current->next;
    }
    oss << "]";
    return oss.str();
}

#endif  // DLINKEDLIST_H
