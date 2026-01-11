#ifndef LISTHEADER_H
#define LISTHEADER_H

#include "DLinkedList.h"
#include "XArrayList.h"

template <class T>
using xvector = XArrayList<T>;

template <class T>
using xlist = DLinkedList<T>;

#endif  // LISTHEADER_H
