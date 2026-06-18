#ifndef READER_H
#define READER_H

#include <string>
#include "BorrowRecord.h"
#include "../datastructures/LinkedList.h"

class Reader
{
private:
    std::string id; // Primary key

public:
    std::string name;
    int type; // Value List

    LinkedList<BorrowRecord *> borrowList;

    // Constructor
    Reader(std::string _id, std::string _name, int _type)
        : id(_id), name(_name), type(_type) {}

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

    // Helper function: Handle Value List
    std::string getTypeText() const
    {
        switch (type)
        {
        case 0:
            return "Sinh viên";
        case 1:
            return "Giảng viên";
        default:
            return "Không xác định"; // UI safety
        }
    }

    // Overload operator==
    bool operator==(const Reader &other) const
    {
        return this->id == other.id;
    }
};

#endif // READER_H