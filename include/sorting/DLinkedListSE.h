#ifndef DLINKEDLISTSE_H
#define DLINKEDLISTSE_H

#include "list/DLinkedList.h"
#include "sorting/ISort.h"

template <class T>
class DLinkedListSE : public DLinkedList<T> {
   public:
    DLinkedListSE(void (*removeData)(DLinkedList<T> *) = 0, bool (*itemEQ)(T &, T &) = 0)
        : DLinkedList<T>(removeData, itemEQ){};

    DLinkedListSE(const DLinkedList<T> &list) { this->copyFrom(list); }

    void sort(int (*comparator)(T &, T &) = 0) {
        if (this->head->next != this->tail) {
            this->tail->prev->next = nullptr;
            this->tail->prev = nullptr;

            typename DLinkedList<T>::Node *newHead = mergeSort(this->head->next, comparator);
            this->head->next = newHead;
            newHead->prev = this->head;

            typename DLinkedList<T>::Node *current = newHead;
            while (current->next != nullptr)
                current = current->next;
            current->next = this->tail;
            this->tail->prev = current;
        }
    }

   protected:
    static int compare(T &lhs, T &rhs, int (*comparator)(T &, T &) = 0) {
        if (comparator != 0)
            return comparator(lhs, rhs);
        else {
            if (lhs < rhs)
                return -1;
            else if (lhs > rhs)
                return +1;
            else
                return 0;
        }
    }

    typename DLinkedList<T>::Node *merge(typename DLinkedList<T>::Node *a,
                                         typename DLinkedList<T>::Node *b,
                                         int (*comparator)(T &, T &) = 0) {
        typename DLinkedList<T>::Node dummyHead;
        typename DLinkedList<T>::Node *current = &dummyHead;

        while (a && b) {
            if (compare(a->data, b->data, comparator) <= 0) {
                current->next = a;
                a->prev = current;
                a = a->next;
            } else {
                current->next = b;
                b->prev = current;
                b = b->next;
            }
            current = current->next;
        }

        if (a) {
            current->next = a;
            a->prev = current;
        }

        if (b) {
            current->next = b;
            b->prev = current;
        }

        return dummyHead.next;
    }

    typename DLinkedList<T>::Node *mergeSort(typename DLinkedList<T>::Node *head,
                                             int (*comparator)(T &, T &) = 0) {
        if ((head == nullptr) || (head->next == nullptr))
            return head;

        // Find middle node
        typename DLinkedList<T>::Node *slow = head;
        typename DLinkedList<T>::Node *fast = head->next;
        while ((fast != nullptr) && (fast->next != nullptr)) {
            slow = slow->next;
            fast = fast->next->next;
        }

        typename DLinkedList<T>::Node *next = slow->next;
        slow->prev = nullptr;
        slow->next = nullptr;

        typename DLinkedList<T>::Node *head1 = mergeSort(head, comparator);
        typename DLinkedList<T>::Node *head2 = mergeSort(next, comparator);
        return merge(head1, head2, comparator);
    }
};

#endif  // DLINKEDLISTSE_H
