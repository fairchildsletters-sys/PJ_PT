#ifndef BOOK_H
#define BOOK_H

#include <string>

class Book
{
private:
    std::string id; // Primary key

public:
    std::string title;
    std::string author;
    std::string genre;
    int status; // Value List

    int totalQuantity;
    int availableQuantity;
    int borrowCount; // Borrow History

    // Constructor
    Book(std::string _id, std::string _title, std::string _author, std::string _genre, int _status, int _qty)
        : id(_id), title(_title), author(_author), genre(_genre), status(_status),
          totalQuantity(_qty), availableQuantity(_qty), borrowCount(0) {}

    // Getters
    std::string getId() const
    {
        return id;
    }

    // Helper function: Handle Value List
    std::string getStatusText() const
    {
        switch (status)
        {
        case 1:
            return "Được phép mượn về";
        case 2:
            return "Chỉ đọc tại chỗ";
        case 3:
            return "Đang bảo trì";
        default:
            return "Không xác định"; // UI safety
        }
    }

    // Overload operator==
    bool operator==(const Book &other) const
    {
        return this->id == other.id;
    }
};

#endif // BOOK_H