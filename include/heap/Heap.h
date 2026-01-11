#ifndef HEAP_H
#define HEAP_H

#include <memory.h>

#include <sstream>

#include "heap/IHeap.h"

template <class T>
class Heap : public IHeap<T> {
   public:
    class Iterator;

   protected:
    T *elements;
    int capacity;
    int count;

    /*
    Function pointer (*comparator)(T &lhs, T&rhs): return: sign of (lhs - rhs)
        -1: lhs < rhs
        0 : lhs == rhs
        +1: ls > rhs
    */
    int (*comparator)(T &lhs, T &rhs);
    void (*deleteUserData)(Heap<T> *pHeap);

   public:
    Heap(int (*comparator)(T &, T &) = 0, void (*deleteUserData)(Heap<T> *) = 0);
    Heap(const Heap<T> &heap);
    Heap<T> &operator=(const Heap<T> &heap);
    ~Heap();

    // Inherit from IHeap
    void push(T item);
    T pop();
    const T peek();
    void remove(T item, void (*removeItemData)(T) = 0);
    bool contains(T item);
    int size();
    void heapify(T array[], int size);
    void clear();
    bool empty();
    string toString(string (*item2str)(T &) = 0);

    void println(string (*item2str)(T &) = 0) { cout << toString(item2str) << endl; }
    Iterator begin() { return Iterator(this, true); }
    Iterator end() { return Iterator(this, false); }

   public:
    /*
    If T is pointer type, pass the address of method "free" to Heap<T>'s constructor to remove the
    user's data (if needed) Example: Heap<Point*> heap(&Heap<Point*>::free) => Destructor will call
    free via function pointer "deleteUserData"
    */
    static void free(Heap<T> *pHeap) {
        for (int idx = 0; idx < pHeap->count; idx++)
            delete pHeap->elements[idx];
    }

   private:
    bool aLTb(T &a, T &b) { return compare(a, b) < 0; }
    int compare(T &a, T &b) {
        if (comparator != 0)
            return comparator(a, b);
        else {
            if (a < b)
                return -1;
            else if (a > b)
                return 1;
            else
                return 0;
        }
    }

    void ensureCapacity(int minCapacity);
    void swap(int a, int b);
    void reheapUp(int position);
    void reheapDown(int position);
    int getItem(T item);
    void removeInternalData();
    void copyFrom(const Heap<T> &heap);

   public:
    class Iterator {
       private:
        Heap<T> *heap;
        int cursor;

       public:
        Iterator(Heap<T> *heap = 0, bool begin = 0) {
            this->heap = heap;
            if (begin && (heap != 0))
                cursor = 0;
            if (!begin && (heap != 0))
                cursor = heap->size();
        }

        Iterator &operator=(const Iterator &iterator) {
            this->heap = iterator.heap;
            this->cursor = iterator.cursor;
            return *this;
        }

        T &operator*() { return this->heap->elements[cursor]; }

        bool operator!=(const Iterator &iterator) { return this->cursor != iterator.cursor; }

        // Prefix ++ overload
        Iterator &operator++() {
            cursor++;
            return *this;
        }

        // Postfix ++ overload
        Iterator operator++(int) {
            Iterator iterator = *this;
            ++*this;
            return iterator;
        }

        void remove(void (*removeItemData)(T) = 0) {
            this->heap->remove(this->heap->elements[cursor], removeItemData);
        }
    };
};

template <class T>
Heap<T>::Heap(int (*comparator)(T &, T &), void (*deleteUserData)(Heap<T> *)) {
    capacity = 10;
    count = 0;
    this->comparator = comparator;
    this->deleteUserData = deleteUserData;
    elements = new T[capacity];
}

template <class T>
Heap<T>::Heap(const Heap<T> &heap) {
    copyFrom(heap);
    deleteUserData = nullptr;
}

template <class T>
Heap<T> &Heap<T>::operator=(const Heap<T> &heap) {
    if (this == &heap)
        return *this;

    removeInternalData();
    copyFrom(heap);
    deleteUserData = nullptr;
    return *this;
}

template <class T>
Heap<T>::~Heap() {
    removeInternalData();
}

template <class T>
void Heap<T>::push(T item) {
    ensureCapacity(count + 1);
    elements[count] = item;
    reheapUp(count);
    count++;
}

template <class T>
T Heap<T>::pop() {
    if (count == 0)
        throw std::underflow_error("Calling to peek with the empty heap.");

    T value = elements[0];
    elements[0] = elements[count - 1];
    count--;
    reheapDown(0);
    return value;
}

template <class T>
const T Heap<T>::peek() {
    if (count == 0)
        throw std::underflow_error("Calling to peek with the empty heap.");
    return elements[0];
}

template <class T>
void Heap<T>::remove(T item, void (*removeItemData)(T)) {
    int index = getItem(item);
    if (index == -1)
        return;

    if (removeItemData != 0)
        removeItemData(elements[index]);

    elements[index] = elements[--count];
    reheapDown(index);
}

template <class T>
bool Heap<T>::contains(T item) {
    return getItem(item) != -1;
}

template <class T>
int Heap<T>::size() {
    return count;
}

template <class T>
void Heap<T>::heapify(T array[], int size) {
    clear();
    ensureCapacity(size);
    for (int i = 0; i < size; i++)
        push(array[i]);
}

template <class T>
void Heap<T>::clear() {
    removeInternalData();
    capacity = 10;
    count = 0;
    elements = new T[capacity];
}

template <class T>
bool Heap<T>::empty() {
    return count == 0;
}

template <class T>
string Heap<T>::toString(string (*item2str)(T &)) {
    stringstream os;
    if (item2str != 0) {
        os << "[";
        for (int idx = 0; idx < count - 1; idx++)
            os << item2str(elements[idx]) << ",";
        if (count > 0)
            os << item2str(elements[count - 1]);
        os << "]";
    } else {
        os << "[";
        for (int idx = 0; idx < count - 1; idx++)
            os << elements[idx] << ",";
        if (count > 0)
            os << elements[count - 1];
        os << "]";
    }
    return os.str();
}

template <class T>
void Heap<T>::ensureCapacity(int minCapacity) {
    if (minCapacity >= capacity) {
        int old_capacity = capacity;
        capacity = old_capacity + (old_capacity >> 2);
        try {
            T *new_data = new T[capacity];
            // OLD: memcpy(new_data, elements, capacity*sizeof(T));
            memcpy(new_data, elements, old_capacity * sizeof(T));
            delete[] elements;
            elements = new_data;
        } catch (std::bad_alloc e) {
            e.what();
        }
    }
}

template <class T>
void Heap<T>::swap(int a, int b) {
    T temp = this->elements[a];
    this->elements[a] = this->elements[b];
    this->elements[b] = temp;
}

template <class T>
void Heap<T>::reheapUp(int position) {
    while (position > 0) {
        int parent = (position - 1) / 2;
        if (parent >= 0) {
            if (aLTb(elements[parent], elements[position]))
                break;
            swap(position, parent);
            position = parent;
        }
    }
}

template <class T>
void Heap<T>::reheapDown(int position) {
    while (position < count / 2) {
        int left = 2 * position + 1;
        int right = 2 * position + 2;
        int smallest = left;

        if (right < count && aLTb(elements[right], elements[left]))
            smallest = right;

        if (aLTb(elements[position], elements[smallest]))
            break;

        swap(position, smallest);
        position = smallest;
    }
}

template <class T>
int Heap<T>::getItem(T item) {
    for (int i = 0; i < count; i++)
        if (compare(elements[i], item) == 0)
            return i;
    return -1;
}

template <class T>
void Heap<T>::removeInternalData() {
    if (this->deleteUserData != 0)
        deleteUserData(this);
    delete[] elements;
}

template <class T>
void Heap<T>::copyFrom(const Heap<T> &heap) {
    this->capacity = heap.capacity;
    this->count = heap.count;
    elements = new T[capacity];
    this->comparator = heap.comparator;
    this->deleteUserData = heap.deleteUserData;
    for (int idx = 0; idx < heap.count; idx++)
        this->elements[idx] = heap.elements[idx];
}

#endif  // HEAP_H
