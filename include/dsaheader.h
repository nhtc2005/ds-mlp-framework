#ifndef DSAHEADER_H
#define DSAHEADER_H

#include "hash/xMap.h"
#include "list/DLinkedList.h"
#include "list/XArrayList.h"

template <class T>
using xvector = XArrayList<T>;

template <class T>
using xlist = DLinkedList<T>;

template <class K, class V>
using xmap = xMap<K, V>;

#endif  // DSAHEADER_H
