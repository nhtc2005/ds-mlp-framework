#ifndef XARRAYLIST_H
#define XARRAYLIST_H

#include <memory.h>

#include <iostream>
#include <sstream>
#include <type_traits>

#include "list/IList.h"

using namespace std;

template <class T>
class XArrayList : public IList<T> {
   public:
    class Iterator;  // Forward declaration

   protected:
    T *data;
    int capacity;
    int count;
    bool (*itemEqual)(T &lhs,
                      T &rhs);  // Function pointer: test if two items (type: T&) are equal or not
    void (*deleteUserData)(
        XArrayList<T> *);  // Function pointer: be called to remove items (if they are pointer type)

   public:
    XArrayList(void (*deleteUserData)(XArrayList<T> *) = 0, bool (*itemEqual)(T &, T &) = 0,
               int capacity = 10);
    XArrayList(const XArrayList<T> &list);
    XArrayList<T> &operator=(const XArrayList<T> &list);
    ~XArrayList();

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
    void setDeleteUserDataPtr(void (*deleteUserData)(XArrayList<T> *) = 0) {
        this->deleteUserData = deleteUserData;
    }
    Iterator begin() { return Iterator(this, 0); }
    Iterator end() { return Iterator(this, count); }

    /*
    free: If T is pointer type, pass the address of method "free" to XArrayList<T>'s constructor to
    remove the user's data (if needed) Example: XArrayList<Point*> list(&XArrayList<Point*>::free)
    => Destructor will call free via function pointer "deleteUserData"
    */
    static void free(XArrayList<T> *list) {
        typename XArrayList<T>::Iterator it = list->begin();
        while (it != list->end()) {
            delete *it;
            it++;
        }
    }

   protected:
    void checkIndex(int index);      // Check validity of index for accessing
    void ensureCapacity(int count);  // Auto-allocate if needed

    /*
    equals:
     - if T is primitive type:
            indexOf and contains will use native operator == to compare two items of T type
     - if T: object type:
            indexOf and contains will use native operator == to compare two items of T type
            Therefore, class of type T MUST override operator ==
     - if T: pointer type:
            indexOf and contains will use function pointer "itemEqual" to compare two items of T
    type Therefore: (1): Must pass itemEqual to the constructor of XArrayList (2): Must define a
    method for comparing the content pointed by two pointers of type T
    */
    static bool equals(T &lhs, T &rhs, bool (*itemEqual)(T &, T &)) {
        if (itemEqual == 0)
            return lhs == rhs;
        else
            return itemEqual(lhs, rhs);
    }

   public:
    class Iterator {
       private:
        int cursor;
        XArrayList<T> *pList;

       public:
        Iterator(XArrayList<T> *pList = 0, int index = 0) {
            this->pList = pList;
            this->cursor = index;
        }

        Iterator &operator=(const Iterator &iterator) {
            cursor = iterator.cursor;
            pList = iterator.pList;
            return *this;
        }

        void remove(void (*removeItemData)(T) = 0) {
            T item = pList->removeAt(cursor);
            if (removeItemData != 0)
                removeItemData(item);
            cursor--;  // Must keep index of previous, for ++ later
        }

        T &operator*() { return pList->data[cursor]; }

        bool operator!=(const Iterator &iterator) { return cursor != iterator.cursor; }

        // Prefix ++ overload
        Iterator &operator++() {
            this->cursor++;
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

template <class T>
XArrayList<T>::XArrayList(void (*deleteUserData)(XArrayList<T> *), bool (*itemEqual)(T &, T &),
                          int capacity)
    : capacity(capacity), count(0), deleteUserData(deleteUserData), itemEqual(itemEqual) {
    data = new T[capacity];
}

template <class T>
XArrayList<T>::XArrayList(const XArrayList<T> &list) {
    this->capacity = list.capacity;
    this->count = list.count;
    this->data = new T[this->capacity];
    for (int i = 0; i < list.count; i++)
        this->data[i] = list.data[i];
    this->itemEqual = list.itemEqual;
    this->deleteUserData = list.deleteUserData;
}

template <class T>
XArrayList<T> &XArrayList<T>::operator=(const XArrayList<T> &list) {
    if (this == &list)  // Check for self-assignment
        return *this;

    // Release allocating memory
    if (deleteUserData != nullptr)  // Check if deleteUserData is provided
        deleteUserData(this);
    delete[] data;

    this->capacity = list.capacity;
    this->count = list.count;
    this->data = new T[this->capacity];
    for (int i = 0; i < list.count; i++)
        this->data[i] = list.data[i];
    this->itemEqual = list.itemEqual;
    this->deleteUserData = list.deleteUserData;
    return *this;
}

template <class T>
XArrayList<T>::~XArrayList() {
    if (deleteUserData != nullptr)
        deleteUserData(this);
    delete[] data;
    data = nullptr;
}

template <class T>
void XArrayList<T>::checkIndex(int index) {
    if ((index < 0) || (index >= count))
        throw std::out_of_range("Index is out of range!");
}

template <class T>
void XArrayList<T>::ensureCapacity(int count) {
    if (count > capacity)  // In this situation, we cannot add new element to the list
    {
        try {
            int newCapacity = capacity * 2 + 1;
            T *newData = new T[newCapacity];

            // Copy elements from old array to new array
            for (int i = 0; i < this->count; i++)
                newData[i] = data[i];

            // Release the allocated memory
            delete[] data;

            // Update new data and capacity
            data = newData;
            capacity = newCapacity;
        } catch (const std::bad_alloc &e) {
            std::cerr << "Memory allocation failed: " << e.what() << '\n';
        }
    }
}

template <class T>
void XArrayList<T>::add(T e) {
    ensureCapacity(count + 1);
    data[count] = e;
    count++;
}

template <class T>
void XArrayList<T>::add(int index, T e) {
    if ((index < 0) || (index > count))
        throw std::out_of_range("Index is out of range!");
    ensureCapacity(count + 1);

    // Shift elements from index to the end to the right
    for (int i = count; i > index; i--)
        data[i] = data[i - 1];

    data[index] = e;
    count++;
}

template <class T>
T XArrayList<T>::removeAt(int index) {
    checkIndex(index);
    T deletingData = data[index];
    for (int i = index; i < count - 1; i++)
        data[i] = data[i + 1];
    count--;
    return deletingData;
}

template <class T>
bool XArrayList<T>::removeItem(T item, void (*removeItemData)(T)) {
    for (int i = 0; i < count; i++) {
        if (equals(data[i], item, itemEqual)) {
            if (removeItemData != nullptr)
                removeItemData(data[i]);
            removeAt(i);
            return true;
        }
    }
    return false;
}

template <class T>
bool XArrayList<T>::empty() {
    if (count == 0)
        return true;
    return false;
}

template <class T>
int XArrayList<T>::size() {
    return count;
}

template <class T>
void XArrayList<T>::clear() {
    if (deleteUserData != nullptr)
        deleteUserData(this);
    delete[] data;
    capacity = 10;
    data = new T[capacity];
    count = 0;
}

template <class T>
T &XArrayList<T>::get(int index) {
    checkIndex(index);
    return data[index];
}

template <class T>
int XArrayList<T>::indexOf(T item) {
    for (int i = 0; i < count; i++)
        if (equals(data[i], item, itemEqual))
            return i;
    return -1;
}

template <class T>
bool XArrayList<T>::contains(T item) {
    return indexOf(item) != -1;
}

template <class T>
string XArrayList<T>::toString(string (*item2str)(T &)) {
    ostringstream oss;
    oss << "[";
    for (int i = 0; i < count; i++) {
        if (item2str != nullptr)
            oss << item2str(data[i]);
        else {
            if constexpr (is_pointer<T>::value)  // If T is pointer
                oss << *data[i];
            else
                oss << data[i];
        }
        if (i < count - 1)
            oss << ", ";
    }
    oss << "]";
    return oss.str();
}

#endif  // XARRAYLIST_H
