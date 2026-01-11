#ifndef XMAP_H
#define XMAP_H

#include <memory.h>

#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#include "hash/IMap.h"
#include "list/DLinkedList.h"

using namespace std;

template <class K, class V>
class xMap : public IMap<K, V> {
   public:
    class Entry;

   protected:
    DLinkedList<Entry *> *table;
    int capacity;
    int count;
    float loadFactor;
    int (*hashCode)(K &, int);
    bool (*keyEqual)(K &, K &);
    bool (*valueEqual)(V &, V &);
    void (*deleteKeys)(xMap<K, V> *);
    void (*deleteValues)(xMap<K, V> *);

   public:
    xMap(int (*hashCode)(K &, int), float loadFactor = 0.75f, bool (*valueEqual)(V &, V &) = 0,
         void (*deleteValues)(xMap<K, V> *) = 0, bool (*keyEqual)(K &, K &) = 0,
         void (*deleteKeys)(xMap<K, V> *) = 0);
    xMap(const xMap<K, V> &map);
    xMap<K, V> &operator=(const xMap<K, V> &map);
    ~xMap();

    // Inherit from IMap
    V put(K key, V value);
    V &get(K key);
    V remove(K key, void (*deleteKeyInMap)(K) = 0);
    bool remove(K key, V value, void (*deleteKeyInMap)(K) = 0, void (*deleteValueInMap)(V) = 0);
    bool containsKey(K key);
    bool containsValue(V value);
    bool empty();
    int size();
    void clear();
    string toString(string (*key2str)(K &) = 0, string (*value2str)(V &) = 0);
    DLinkedList<K> keys();
    DLinkedList<V> values();
    DLinkedList<int> clashes();

    void println(string (*key2str)(K &) = 0, string (*value2str)(V &) = 0) {
        cout << this->toString(key2str, value2str) << endl;
    }

    int getCapacity() { return capacity; }

    static int intKeyHash(int &key, int capacity) { return key % capacity; }

    static int stringKeyHash(string &key, int capacity) {
        long long int sum = 0;
        for (int idx = 0; idx < key.length(); idx++)
            sum += key[idx];
        return sum % capacity;
    }

    /*
    freeKey(xMap<K, V> *pMap):
        To delete keys stored in map
        When to use: K is a pointer type, and users need xMap to free keys
    */
    static void freeKey(xMap<K, V> *pMap) {
        for (int idx = 0; idx < pMap->capacity; idx++) {
            DLinkedList<Entry *> list = pMap->table[idx];
            for (auto pEntry : list)
                delete pEntry->key;
        }
    }

    /*
    freeValue(xMap<K, V> *pMap):
        To delete values stored in map
        When to use: V is a pointer type, and users need xMap to free values
    */
    static void freeValue(xMap<K, V> *pMap) {
        for (int idx = 0; idx < pMap->capacity; idx++) {
            DLinkedList<Entry *> list = pMap->table[idx];
            for (auto pEntry : list)
                delete pEntry->value;
        }
    }

    // To delete pointer to Entry
    static void deleteEntry(Entry *ptr) { delete ptr; }

   protected:
    void ensureLoadFactor(int minCapacity);
    void rehash(int newCapacity);
    void removeInternalData();
    void copyMapFrom(const xMap<K, V> &map);
    void moveEntries(DLinkedList<Entry *> *oldTable, int oldCapacity,
                     DLinkedList<Entry *> *newTable, int newCapacity);

    bool keyEQ(K &lhs, K &rhs) {
        if (keyEqual != 0)
            return keyEqual(lhs, rhs);
        else
            return lhs == rhs;
    }

    bool valueEQ(V &lhs, V &rhs) {
        if (valueEqual != 0)
            return valueEqual(lhs, rhs);
        else
            return lhs == rhs;
    }

   public:
    class Entry {
        friend class xMap<K, V>;

       private:
        K key;
        V value;

       public:
        Entry(K key, V value) {
            this->key = key;
            this->value = value;
        }
    };
};

template <class K, class V>
xMap<K, V>::xMap(int (*hashCode)(K &, int), float loadFactor, bool (*valueEqual)(V &lhs, V &rhs),
                 void (*deleteValues)(xMap<K, V> *), bool (*keyEqual)(K &lhs, K &rhs),
                 void (*deleteKeys)(xMap<K, V> *pMap)) {
    this->capacity = 10;
    this->count = 0;
    this->loadFactor = loadFactor;
    this->hashCode = hashCode;
    this->keyEqual = keyEqual;
    this->valueEqual = valueEqual;
    this->deleteKeys = deleteKeys;
    this->deleteValues = deleteValues;
    this->table = new DLinkedList<Entry *>[capacity];
}

template <class K, class V>
xMap<K, V>::xMap(const xMap<K, V> &map) {
    // In some case, this->capacity = map->capacity can lead to unexpected rehash
    this->capacity = 10;
    this->count = 0;
    this->table = new DLinkedList<Entry *>[capacity];
    this->loadFactor = map.loadFactor;
    this->hashCode = map.hashCode;
    this->keyEqual = map.keyEqual;
    this->valueEqual = map.valueEqual;
    this->deleteKeys = nullptr;    // To avoid double free because it is just shallow copy
    this->deleteValues = nullptr;  // To avoid double free because it is just shallow copy

    // Copy entries
    for (int idx = 0; idx < map.capacity; idx++) {
        DLinkedList<Entry *> &list = map.table[idx];
        for (auto pEntry : list)
            this->put(pEntry->key, pEntry->value);
    }
}

template <class K, class V>
xMap<K, V> &xMap<K, V>::operator=(const xMap<K, V> &map) {
    if (this != &map) {
        copyMapFrom(map);
        this->deleteKeys = nullptr;
        this->deleteValues = nullptr;
    }
    return *this;
}

template <class K, class V>
xMap<K, V>::~xMap() {
    removeInternalData();
}

template <class K, class V>
V xMap<K, V>::put(K key, V value) {
    int index = this->hashCode(key, capacity);
    V retValue;
    DLinkedList<Entry *> &list = table[index];
    bool found = false;

    for (auto pEntry : list) {
        if (keyEQ(pEntry->key, key)) {
            retValue = pEntry->value;
            pEntry->value = value;
            found = true;
            break;
        }
    }

    if (!found) {
        retValue = value;
        Entry *newEntry = new Entry(key, value);
        list.add(newEntry);
        count++;
        ensureLoadFactor(count);
    }

    return retValue;
}

template <class K, class V>
V &xMap<K, V>::get(K key) {
    int index = hashCode(key, capacity);
    DLinkedList<Entry *> &list = table[index];

    for (auto pEntry : list)
        if (keyEQ(pEntry->key, key))
            return pEntry->value;

    // If key is not found
    stringstream os;
    os << "key (" << key << ") is not found";
    throw KeyNotFound(os.str());
}

template <class K, class V>
V xMap<K, V>::remove(K key, void (*deleteKeyInMap)(K)) {
    int index = hashCode(key, capacity);
    DLinkedList<Entry *> &list = table[index];

    for (auto pEntry : list) {
        if (keyEQ(pEntry->key, key)) {
            V retValue = pEntry->value;
            if (deleteKeyInMap != 0)
                deleteKeyInMap(pEntry->key);
            list.removeItem(pEntry, xMap<K, V>::deleteEntry);
            count--;
            return retValue;
        }
    }

    // If key is not found
    stringstream os;
    os << "key (" << key << ") is not found";
    throw KeyNotFound(os.str());
}

template <class K, class V>
bool xMap<K, V>::remove(K key, V value, void (*deleteKeyInMap)(K), void (*deleteValueInMap)(V)) {
    int index = hashCode(key, capacity);
    DLinkedList<Entry *> &list = table[index];

    for (auto pEntry : list) {
        if (keyEQ(pEntry->key, key) && valueEQ(pEntry->value, value)) {
            if (deleteKeyInMap != 0)
                deleteKeyInMap(pEntry->key);
            if (deleteValueInMap != 0)
                deleteValueInMap(pEntry->value);
            list.removeItem(pEntry, xMap<K, V>::deleteEntry);
            count--;
            return true;
        }
    }

    return false;
}

template <class K, class V>
bool xMap<K, V>::containsKey(K key) {
    int index = hashCode(key, capacity);
    DLinkedList<Entry *> &list = table[index];

    for (auto pEntry : list)
        if (keyEQ(pEntry->key, key))
            return true;

    return false;
}

template <class K, class V>
bool xMap<K, V>::containsValue(V value) {
    for (int i = 0; i < capacity; i++) {
        DLinkedList<Entry *> &list = table[i];
        for (auto pEntry : list)
            if (valueEQ(pEntry->value, value))
                return true;
    }
    return false;
}

template <class K, class V>
bool xMap<K, V>::empty() {
    return count == 0;
}

template <class K, class V>
int xMap<K, V>::size() {
    return count;
}

template <class K, class V>
void xMap<K, V>::clear() {
    removeInternalData();
    count = 0;
    capacity = 10;
    table = new DLinkedList<Entry *>[capacity];
}

template <class K, class V>
DLinkedList<K> xMap<K, V>::keys() {
    DLinkedList<K> keys;
    for (int i = 0; i < capacity; i++) {
        DLinkedList<Entry *> &list = table[i];
        for (auto pEntry : list)
            keys.add(pEntry->key);
    }
    return keys;
}

template <class K, class V>
DLinkedList<V> xMap<K, V>::values() {
    DLinkedList<V> values;
    for (int i = 0; i < capacity; i++) {
        DLinkedList<Entry *> &list = table[i];
        for (auto pEntry : list)
            values.add(pEntry->value);
    }
    return values;
}

template <class K, class V>
DLinkedList<int> xMap<K, V>::clashes() {
    DLinkedList<int> clashes;
    for (int i = 0; i < capacity; i++) {
        DLinkedList<Entry *> &list = table[i];
        clashes.add(list.size());
    }
    return clashes;
}

template <class K, class V>
string xMap<K, V>::toString(string (*key2str)(K &), string (*value2str)(V &)) {
    stringstream os;
    string mark(50, '=');
    os << mark << endl;
    os << setw(12) << left << "capacity: " << capacity << endl;
    os << setw(12) << left << "size: " << count << endl;
    for (int idx = 0; idx < capacity; idx++) {
        DLinkedList<Entry *> list = table[idx];
        os << setw(4) << left << idx << ": ";
        stringstream itemos;
        for (auto pEntry : list) {
            itemos << " (";

            if (key2str != 0)
                itemos << key2str(pEntry->key);
            else
                itemos << pEntry->key;

            itemos << ",";

            if (value2str != 0)
                itemos << value2str(pEntry->value);
            else
                itemos << pEntry->value;

            itemos << ");";
        }
        string valuestr = itemos.str();
        if (valuestr.length() > 0)
            valuestr = valuestr.substr(0, valuestr.length() - 1);
        os << valuestr << endl;
    }
    os << mark << endl;
    return os.str();
}

template <class K, class V>
void xMap<K, V>::moveEntries(DLinkedList<Entry *> *oldTable, int oldCapacity,
                             DLinkedList<Entry *> *newTable, int newCapacity) {
    for (int old_index = 0; old_index < oldCapacity; old_index++) {
        DLinkedList<Entry *> &oldList = oldTable[old_index];
        for (auto oldEntry : oldList) {
            int new_index = this->hashCode(oldEntry->key, newCapacity);
            DLinkedList<Entry *> &newList = newTable[new_index];
            newList.add(oldEntry);
        }
    }
}

template <class K, class V>
void xMap<K, V>::ensureLoadFactor(int current_size) {
    int maxSize = (int)(loadFactor * capacity);
    if (current_size > maxSize) {
        int oldCapacity = capacity;
        int newCapacity = 1.5 * oldCapacity;
        rehash(newCapacity);
    }
}

template <class K, class V>
void xMap<K, V>::rehash(int newCapacity) {
    DLinkedList<Entry *> *pOldMap = this->table;
    int oldCapacity = capacity;

    // Create new table:
    this->table = new DLinkedList<Entry *>[newCapacity];
    this->capacity = newCapacity;

    moveEntries(pOldMap, oldCapacity, this->table, newCapacity);

    // Remove old data: Only remove nodes in list, no entry
    for (int idx = 0; idx < oldCapacity; idx++) {
        DLinkedList<Entry *> &list = pOldMap[idx];
        list.clear();
    }

    // Remove oldTable
    delete[] pOldMap;
}

template <class K, class V>
void xMap<K, V>::removeInternalData() {
    // Remove user's data
    if (deleteKeys != 0)
        deleteKeys(this);
    if (deleteValues != 0)
        deleteValues(this);

    // Remove all entries in the current map
    for (int idx = 0; idx < this->capacity; idx++) {
        DLinkedList<Entry *> &list = this->table[idx];
        for (auto pEntry : list)
            delete pEntry;
        list.clear();
    }

    // Remove table
    delete[] table;
}

template <class K, class V>
void xMap<K, V>::copyMapFrom(const xMap<K, V> &map) {
    removeInternalData();

    this->capacity = 10;
    this->count = 0;
    this->table = new DLinkedList<Entry *>[capacity];
    this->loadFactor = map.loadFactor;
    this->hashCode = map.hashCode;
    this->keyEqual = map.keyEqual;
    this->valueEqual = map.valueEqual;
    // SHOULD NOT COPY: deleteKeys, deleteValues => delete ONLY TIME in map if needed

    // Copy entries
    for (int idx = 0; idx < map.capacity; idx++) {
        DLinkedList<Entry *> &list = map.table[idx];
        for (auto pEntry : list)
            this->put(pEntry->key, pEntry->value);
    }
}

#endif  // XMAP_H
