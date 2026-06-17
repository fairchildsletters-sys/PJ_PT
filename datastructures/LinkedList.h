#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include "Node.h"
#include <cstddef>
#include <stdexcept>

template <typename T>
class LinkedList
{
private:
    Node<T> *head;
    size_t size;

public:
    LinkedList() : head(nullptr), size(0) {}

    ~LinkedList()
    {
        clear();
    }

    // Disable copy constructor and assignment operator to avoid double-free bugs
    LinkedList(const LinkedList &) = delete;
    LinkedList &operator=(const LinkedList &) = delete;

    void clear()
    {
        Node<T> *current = head;
        while (current != nullptr)
        {
            Node<T> *temp = current;
            current = current->next;
            delete temp;
        }
        head = nullptr;
        size = 0;
    }

    void insertAtHead(const T &val)
    {
        Node<T> *newNode = new Node<T>(val, head);
        head = newNode;
        size++;
    }

    bool remove(const T &val)
    {
        if (head == nullptr)
            return false;

        // Minor Case: Removing the first element (head)
        if (head->data == val)
        {
            Node<T> *temp = head;
            head = head->next;
            delete temp;
            size--;
            return true;
        }

        // Removing an element from the middle/end of the list
        Node<T> *current = head;
        while (current->next != nullptr)
        {
            if (current->next->data == val)
            {
                Node<T> *temp = current->next;
                current->next = temp->next;
                delete temp;
                size--;
                return true;
            }
            current = current->next;
        }

        return false; // Element to delete not found
    }

    Node<T> *getHead() const
    {
        return head;
    }

    size_t getSize() const
    {
        return size;
    }

    bool isEmpty() const
    {
        return size == 0;
    }
};

#endif // LINKEDLIST_H