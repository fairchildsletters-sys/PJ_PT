#ifndef BORROW_RECORD_H
#define BORROW_RECORD_H

#include <string>
#include <ctime> // To process time

struct BorrowRecord
{
    std::string recordId; // Partial key
    std::string bookId;   // Within a reader's borrow list

    time_t borrowDate;
    time_t dueDate;

    // Constructor
    BorrowRecord(std::string _recordId, std::string _bookId, time_t _borrowDate, time_t _dueDate)
        : recordId(_recordId), bookId(_bookId), borrowDate(_borrowDate), dueDate(_dueDate) {}

    // Overload operator==
    bool operator==(const BorrowRecord &other) const
    {
        return this->recordId == other.recordId;
    }
};

#endif // BORROW_RECORD_H