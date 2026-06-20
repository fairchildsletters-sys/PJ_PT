#ifndef BOOK_H
#define BOOK_H

#include <string>

enum class BookStatus
{
    AVAILABLE,
    READ_ON_SITE,
    MAINTENANCE
};

class Book
{
private:
    std::string id; // Primary key

public:
    std::string title;
    std::string author;
    std::string genre;
    BookStatus status;

    int totalQuantity;
    int availableQuantity;
    int borrowCount; // Borrow History

    // Constructor
    Book(std::string _id, std::string _title, std::string _author, std::string _genre, BookStatus _status, int _qty)
        : id(_id), title(_title), author(_author), genre(_genre), status(_status),
          totalQuantity(_qty), availableQuantity(_qty), borrowCount(0) {}

    // Copy Constructor
    Book(const Book &other)
        : id(other.id), title(other.title), author(other.author), genre(other.genre),
          status(other.status), totalQuantity(other.totalQuantity),
          availableQuantity(other.availableQuantity), borrowCount(other.borrowCount) {}

    // Getters
    std::string getId() const
    {
        return id;
    }

    // Overload operator==
    bool operator==(const Book &other) const
    {
        return this->id == other.id;
    }
};

#endif // BOOK_H