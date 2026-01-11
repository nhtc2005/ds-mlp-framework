#ifndef IMAP_H
#define IMAP_H

#include <string>

#include "list/DLinkedList.h"

using namespace std;

class KeyNotFound : public std::exception {
   private:
    string desc;

   public:
    KeyNotFound(string desc) { this->desc = desc; }
    const char* what() const throw() { return desc.c_str(); }
};

template <class K, class V>
struct Pair {
    K key;
    V value;

    Pair(K key, V value) {
        this->key = key;
        this->value = value;
    }

    Pair(const Pair& pair) {
        this->key = pair.key;
        this->value = pair.value;
    }

    Pair& operator=(const Pair& pair) {
        this->key = pair.key;
        this->value = pair.value;
    }
};

template <class K, class V>
class IMap {
   public:
    virtual ~IMap(){};
    virtual V put(K key, V value) = 0;
    virtual V& get(K key) = 0;
    virtual V remove(K key, void (*deleteKeyInMap)(K) = 0) = 0;
    virtual bool remove(K key, V value, void (*deleteKeyInMap)(K) = 0,
                        void (*deleteValueInMap)(V) = 0) = 0;
    virtual bool containsKey(K key) = 0;
    virtual bool containsValue(V value) = 0;
    virtual bool empty() = 0;
    virtual int size() = 0;
    virtual void clear() = 0;
    virtual string toString(string (*key2str)(K&) = 0, string (*value2str)(V&) = 0) = 0;
    virtual DLinkedList<K> keys() = 0;
    virtual DLinkedList<V> values() = 0;
    virtual DLinkedList<int> clashes() = 0;
};

#endif  // IMAP_H
