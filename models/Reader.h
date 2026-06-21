#ifndef READER_H
#define READER_H

#include <string>
#include "BorrowRecord.h"
#include "../datastructures/LinkedList.h"

enum class ReaderType
{
    STUDENT,
    TEACHER
};

class Reader
{
private:
    std::string id; // Primary key

public:
    std::string name;
    ReaderType type;
    long long debt;

    // Only stores active borrow records
    LinkedList<BorrowRecord *> borrowList;

    // Constructor
    Reader(std::string _id,
           std::string _name,
           ReaderType _type,
           long long _debt = 0)
        : id(_id), name(_name), type(_type), debt(_debt) {}

    // Copy Constructor
    Reader(const Reader &other)
        : id(other.id), name(other.name), type(other.type), debt(other.debt)
    {
        Node<BorrowRecord *> *current = other.borrowList.getHead();
        while (current != nullptr)
        {
            if (current->data != nullptr)
            {
                BorrowRecord *clonedRecord = new BorrowRecord(*(current->data));
                this->borrowList.insertAtHead(clonedRecord);
            }
            current = current->next;
        }
    }

    // Destructor
    ~Reader()
    {
        Node<BorrowRecord *> *current = borrowList.getHead();
        while (current != nullptr)
        {
            Node<BorrowRecord *> *nextNode = current->next;
            delete current->data; // Delete records
            current = nextNode;
        }
    }

    // Getters
    std::string getId() const
    {
        return id;
    }

    // Overload operator==
    bool operator==(const Reader &other) const
    {
        return this->id == other.id;
    }
};

#endif // READER_H