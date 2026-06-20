#ifndef LIBRARYMANAGER_H
#define LIBRARYMANAGER_H

#include "../datastructures/LinkedList.h"
#include "../datastructures/HashMap.h"
#include "../models/Book.h"
#include "../models/Reader.h"
#include "../models/BorrowRecord.h"
#include "../utils/functional/ReportStructs.h"
#include <iostream>
#include <iomanip>
#include <string>
#include <ctime>

class LibraryManager
{
private:
    HashMap<std::string, Book *> books;
    HashMap<std::string, Reader *> readers;

    // Business configuration constants
    const size_t MAX_BORROW_SV = 3;
    const size_t MAX_BORROW_GV = 5;
    const int MAX_DAYS_BORROW = 14;
    const long long FINE_PER_DAY = 5000;

public:
    // =======================================================================
    // MODULE 0: SYSTEM FUNCTIONS (SETUP & TEARDOWN)
    // =======================================================================
    LibraryManager() {}

    ~LibraryManager()
    {
        cleanUp();
    }

    // Disable copy constructor and assignment operator
    LibraryManager(const LibraryManager &) = delete;
    LibraryManager &operator=(const LibraryManager &) = delete;

    void cleanUp()
    {
        // 1. Free all Book objects from Heap (Iterator)
        for (auto it = books.begin(); it != books.end(); ++it)
        {
            delete it->value; // Book*
        }
        books.clear();

        // 2. Free all Reader objects from Heap (Iterator)
        for (auto it = readers.begin(); it != readers.end(); ++it)
        {
            delete it->value; // Reader* (handling its own BorrowRecord)
        }
        readers.clear();
    }

    // =======================================================================
    // MODULE 1: CORE DATA OPERATIONS (CRUD)
    // =======================================================================

    // --- BOOK MANAGEMENT ---
    enum class BookOpStatus
    {
        SUCCESS,
        ERR_DUPLICATE_ID,
        ERR_NOT_FOUND,
        ERR_BOOKS_BORROWED
    };

    BookOpStatus addBook(const Book &book)
    {
        Book *newBook = new Book(book);

        if (books.add(book.getId(), newBook))
        {
            return BookOpStatus::SUCCESS;
        }
        else
        {
            delete newBook;
            return BookOpStatus::ERR_DUPLICATE_ID;
        }
    }

    BookOpStatus updateBook(const Book &book)
    {
        auto it = books.find(book.getId());
        if (it == books.end())
        {
            return BookOpStatus::ERR_NOT_FOUND;
        }

        if (it->value->totalQuantity > it->value->availableQuantity)
        {
            return BookOpStatus::ERR_BOOKS_BORROWED;
        }

        // Update allowed attributes
        it->value->title = book.title;
        it->value->author = book.author;
        it->value->genre = book.genre;
        it->value->status = book.status;
        it->value->totalQuantity = book.totalQuantity;
        it->value->availableQuantity = book.totalQuantity;

        return BookOpStatus::SUCCESS;
    }

    BookOpStatus removeBook(const std::string &id)
    {
        auto it = books.find(id);
        if (it == books.end())
        {
            return BookOpStatus::ERR_NOT_FOUND;
        }

        if (it->value->totalQuantity > it->value->availableQuantity)
        {
            return BookOpStatus::ERR_BOOKS_BORROWED;
        }

        delete it->value;
        books.remove(id);
        return BookOpStatus::SUCCESS;
    }

    Book *findBookById(const std::string &id) const
    {
        auto it = books.find(id);
        if (it == books.end())
        {
            return nullptr;
        }
        return it->value;
    }

    // Search books by other attributes (Linear)
    enum class BookSearchCriteria
    {
        TITLE,
        AUTHOR,
        GENRE
    };

    void searchBooks(const std::string &keyword, BookSearchCriteria criteria, LinkedList<Book *> &results) const
    {
        for (auto it = books.begin(); it != books.end(); ++it)
        {
            const std::string *fieldToSearch = nullptr;
            switch (criteria)
            {
            case BookSearchCriteria::TITLE:
                fieldToSearch = &(it->value->title);
                break;
            case BookSearchCriteria::AUTHOR:
                fieldToSearch = &(it->value->author);
                break;
            case BookSearchCriteria::GENRE:
                fieldToSearch = &(it->value->genre);
                break;
            default:
                break;
            }
            if (fieldToSearch != nullptr && fieldToSearch->find(keyword) != std::string::npos)
            {
                results.insertAtHead(it->value);
            }
        }
    }

    // --- READER MANAGEMENT ---
    enum class ReaderOpStatus
    {
        SUCCESS,
        ERR_DUPLICATE_ID,
        ERR_NOT_FOUND,
        ERR_HAS_BORROWED_BOOKS
    };

    ReaderOpStatus addReader(const Reader &reader)
    {
        Reader *newReader = new Reader(reader);

        if (readers.add(reader.getId(), newReader))
        {
            return ReaderOpStatus::SUCCESS;
        }
        else
        {
            delete newReader;
            return ReaderOpStatus::ERR_DUPLICATE_ID;
        }
    }

    ReaderOpStatus updateReader(const Reader &reader)
    {
        auto it = readers.find(reader.getId());
        if (it == readers.end())
        {
            return ReaderOpStatus::ERR_NOT_FOUND;
        }

        // Update allowed attributes
        it->value->name = reader.name;
        it->value->type = reader.type;

        return ReaderOpStatus::SUCCESS;
    }

    ReaderOpStatus removeReader(const std::string &id)
    {
        auto it = readers.find(id);
        if (it == readers.end())
        {
            return ReaderOpStatus::ERR_NOT_FOUND;
        }

        // Only stores active borrow records
        if (!it->value->borrowList.isEmpty())
        {
            return ReaderOpStatus::ERR_HAS_BORROWED_BOOKS;
        }

        delete it->value;
        readers.remove(id);
        return ReaderOpStatus::SUCCESS;
    }

    Reader *findReaderById(const std::string &id) const
    {
        auto it = readers.find(id);
        if (it == readers.end())
        {
            return nullptr;
        }
        return it->value;
    }

    // =======================================================================
    // MODULE 2: BUSINESS LOGIC (BORROW & RETURN)
    // =======================================================================

    // Helper: Check if reader has any overdue books
    bool hasOverdueBooks(Reader *reader, time_t currentDate) const
    {
        Node<BorrowRecord *> *current = reader->borrowList.getHead();
        while (current != nullptr)
        {
            if (current->data->dueDate < currentDate)
            {
                return true;
            }
            current = current->next;
        }
        return false;
    }

    // Helper: Pay debt
    bool payDebt(Reader *reader, long long amount)
    {
        if (amount <= 0 || amount > reader->debt)
            return false; // rejects redundancy
        reader->debt -= amount;
        return true;
    }

    // --- Borrow books ---
    enum class BorrowOpStatus
    {
        SUCCESS,
        ERR_NOT_FOUND,
        ERR_STATUS_NOT_ALLOWED,
        ERR_OUT_OF_STOCK,
        ERR_LIMIT_REACHED,
        ERR_HAS_OVERDUE,
        ERR_HAS_DEBT
    };

    BorrowOpStatus borrowBook(const std::string &readerId, const std::string &bookId, time_t borrowDate)
    {
        Reader *reader = findReaderById(readerId);
        Book *book = findBookById(bookId);

        if (reader == nullptr || book == nullptr)
        {
            return BorrowOpStatus::ERR_NOT_FOUND;
        }

        // Check book status
        if (book->status != BookStatus::AVAILABLE)
        {
            return BorrowOpStatus::ERR_STATUS_NOT_ALLOWED;
        }

        // Check availability
        if (book->availableQuantity <= 0)
        {
            return BorrowOpStatus::ERR_OUT_OF_STOCK;
        }

        // Check borrow limits
        size_t limit = (reader->type == ReaderType::STUDENT) ? MAX_BORROW_SV : MAX_BORROW_GV;
        if (reader->borrowList.getSize() >= limit)
        {
            return BorrowOpStatus::ERR_LIMIT_REACHED;
        }

        // Check for overdue books
        if (hasOverdueBooks(reader, borrowDate))
        {
            return BorrowOpStatus::ERR_HAS_OVERDUE;
        }

        // Check for debt
        if (reader->debt > 0)
        {
            return BorrowOpStatus::ERR_HAS_DEBT;
        }

        // Process borrowing
        time_t dueDate = borrowDate + (MAX_DAYS_BORROW * 86400); // 86400 seconds = 1 day
        std::string recordId = readerId + "_" + bookId + "_" + std::to_string(borrowDate);

        BorrowRecord *record = new BorrowRecord(recordId, bookId, borrowDate, dueDate);

        reader->borrowList.insertAtHead(record);
        book->availableQuantity--;
        book->borrowCount++;

        return BorrowOpStatus::SUCCESS;
    }

    // --- Return books ---
    enum class ReturnOpStatus
    {
        SUCCESS,
        ERR_NOT_FOUND,
        ERR_NOT_BORROWED
    };

    ReturnOpStatus returnBook(const std::string &readerId, const std::string &bookId, time_t returnDate)
    {
        Reader *reader = findReaderById(readerId);
        Book *book = findBookById(bookId);

        if (reader == nullptr || book == nullptr)
        {
            return ReturnOpStatus::ERR_NOT_FOUND;
        }

        // Find the specific borrow record in reader's list
        Node<BorrowRecord *> *current = reader->borrowList.getHead();
        BorrowRecord *targetRecord = nullptr;

        while (current != nullptr)
        {
            if (current->data->bookId == bookId)
            {
                targetRecord = current->data;
                break;
            }
            current = current->next;
        }

        if (targetRecord == nullptr)
        {
            return ReturnOpStatus::ERR_NOT_BORROWED;
        }

        // Calculate fine if overdue
        double diffSeconds = difftime(returnDate, targetRecord->dueDate);
        if (diffSeconds > 0)
        {
            int overdueDays = static_cast<int>(diffSeconds / 86400);
            if (overdueDays > 0)
            {
                long long fine = overdueDays * FINE_PER_DAY;
                reader->debt += fine;
            }
        }

        // Process return
        book->availableQuantity++;
        reader->borrowList.remove(targetRecord);
        delete targetRecord; // Free memory

        return ReturnOpStatus::SUCCESS;
    }

    // =======================================================================
    // MODULE 3: DATA EXTRACTION & STATISTICS
    // =======================================================================

    // To report all currently borrowed books
    bool getCurrentlyBorrowed(LinkedList<BookReport> &reportList) const
    {
        if (reportList.getHead() != nullptr)
        {
            return false;
        } // Meaningfulness

        HashMap<std::string, bool> visitedBorrow;

        for (auto readerIt = readers.begin(); readerIt != readers.end(); ++readerIt)
        {
            Reader *reader = readerIt->value;
            Node<BorrowRecord *> *bNode = reader->borrowList.getHead();

            while (bNode != nullptr)
            {
                std::string bId = bNode->data->bookId;
                auto visitedIt = visitedBorrow.find(bId);

                // If this book has NOT been reported (visited)
                if (visitedIt == visitedBorrow.end())
                {
                    auto bookIt = books.find(bId);

                    if (bookIt != books.end()) // Safety
                    {
                        Book *book = bookIt->value;
                        if (book != nullptr) // Safety
                        {
                            BookReport rep{book->getId(), book->title, book->author, book->genre, 0};
                            reportList.insertAtHead(rep);
                        }
                    }
                    visitedBorrow.add(bId, true); // Mark
                }
                bNode = bNode->next;
            }
        }
        return true;
    }

    // To report all currently overdue books
    bool getOverdue(LinkedList<BookReport> &reportList, time_t currentDate) const
    {
        if (reportList.getHead() != nullptr)
        {
            return false;
        } // Meaningfulness

        HashMap<std::string, bool> visitedOverdue;

        for (auto readerIt = readers.begin(); readerIt != readers.end(); ++readerIt)
        {
            Reader *reader = readerIt->value;
            Node<BorrowRecord *> *bNode = reader->borrowList.getHead();

            while (bNode != nullptr)
            {
                BorrowRecord *rec = bNode->data;

                // Check precise overdue
                if (rec->dueDate < currentDate)
                {
                    std::string bId = rec->bookId;
                    auto visitedIt = visitedOverdue.find(bId);

                    // If this book has NOT been reported (visited)
                    if (visitedIt == visitedOverdue.end())
                    {
                        auto bookIt = books.find(bId);

                        if (bookIt != books.end()) // Safety
                        {
                            Book *book = bookIt->value;
                            if (book != nullptr) // Safety
                            {
                                BookReport rep{book->getId(), book->title, book->author, book->genre, 0};
                                reportList.insertAtHead(rep);
                            }
                        }
                        visitedOverdue.add(bId, true); // Mark
                    }
                }
                bNode = bNode->next;
            }
        }
        return true;
    }

    // To report top borrowed books (for all)
    bool getTopBooks(LinkedList<BookReport> &topList) const
    {
        if (topList.getHead() != nullptr)
        {
            return false;
        } // Meaningfulness

        // Pointer to build a temporary ASCENDING sorted list
        Node<BookReport> *sortedHead = nullptr;

        for (auto it = books.begin(); it != books.end(); ++it)
        {
            Book *book = it->value;
            if (book == nullptr || book->borrowCount <= 0)
                continue; // Filter empty books

            // Encapsulate data
            BookReport rep{book->getId(), book->title, book->author, book->genre, book->borrowCount};
            Node<BookReport> *newNode = new Node<BookReport>(rep);

            // --- INSERTION SORT ALGORITHM (USING POINTERS) ---
            // Sort in ASCENDING order
            if (sortedHead == nullptr || sortedHead->data.borrowCount >= newNode->data.borrowCount)
            {
                // Case: Insert at the beginning of the temporary list
                newNode->next = sortedHead;
                sortedHead = newNode;
            }
            else
            {
                // Reset `curr`, step until it's next node crosses the limit
                Node<BookReport> *curr = sortedHead;
                while (curr->next != nullptr && curr->next->data.borrowCount < newNode->data.borrowCount)
                {
                    curr = curr->next;
                }
                // Insert newNode after curr
                newNode->next = curr->next;
                curr->next = newNode;
            }
        }

        // Transfer data to return
        // REVERSE the temporary list and load into topList
        Node<BookReport> *temp = sortedHead;
        while (temp != nullptr)
        {
            topList.insertAtHead(temp->data);

            // Clean up memory of the temporary list
            Node<BookReport> *toDelete = temp;
            temp = temp->next;
            delete toDelete;
        }

        return true;
    }
};

#endif // LIBRARYMANAGER_H