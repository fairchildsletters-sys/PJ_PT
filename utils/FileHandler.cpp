#include "FileHandler.h"
#include <fstream>
#include <sstream>

namespace FileHandler
{
    // ==========================================================
    // ENUM HELPERS
    // ==========================================================

    static BookStatus stringToBookStatus(const std::string& str)
    {
        if (str == "READ_ON_SITE")
            return BookStatus::READ_ON_SITE;

        if (str == "MAINTENANCE")
            return BookStatus::MAINTENANCE;

        return BookStatus::AVAILABLE;
    }

    static std::string bookStatusToString(BookStatus status)
    {
        switch (status)
        {
        case BookStatus::READ_ON_SITE:
            return "READ_ON_SITE";

        case BookStatus::MAINTENANCE:
            return "MAINTENANCE";

        default:
            return "AVAILABLE";
        }
    }

    static ReaderType stringToReaderType(const std::string& str)
    {
        return (str == "TEACHER")
                   ? ReaderType::TEACHER
                   : ReaderType::STUDENT;
    }

    static std::string readerTypeToString(ReaderType type)
    {
        return (type == ReaderType::TEACHER)
                   ? "TEACHER"
                   : "STUDENT";
    }

    // ==========================================================
    // BOOK
    // ==========================================================

    static bool parseBookLine(const std::string& line,
                              Book*& book)
    {
        std::istringstream iss(line);

        std::string id;
        std::string title;
        std::string author;
        std::string genre;
        std::string statusStr;

        std::string totalStr;
        std::string availStr;
        std::string borrowStr;

        if (!std::getline(iss, id, '|'))
            return false;

        if (!std::getline(iss, title, '|'))
            return false;

        if (!std::getline(iss, author, '|'))
            return false;

        if (!std::getline(iss, genre, '|'))
            return false;

        if (!std::getline(iss, statusStr, '|'))
            return false;

        if (!std::getline(iss, totalStr, '|'))
            return false;

        if (!std::getline(iss, availStr, '|'))
            return false;

        if (!std::getline(iss, borrowStr))
            return false;

        try
        {
            int totalQty = std::stoi(totalStr);
            int availableQty = std::stoi(availStr);
            int borrowCount = std::stoi(borrowStr);

            book = new Book(
                id,
                title,
                author,
                genre,
                stringToBookStatus(statusStr),
                totalQty,
                availableQty,
                borrowCount);

            return true;
        }
        catch (...)
        {
            return false;
        }
    }

    bool loadBooks(const std::string& path,
                   HashMap<std::string, Book*>& books)
    {
        std::ifstream file(path);

        if (!file.is_open())
        {
            std::ofstream create(path);
            create.close();
            return true;
        }

        std::string line;

        while (std::getline(file, line))
        {
            if (line.empty())
                continue;

            Book* book = nullptr;

            if (!parseBookLine(line, book))
            {
                delete book;
                continue;
            }

            if (!books.add(book->getId(), book))
            {
                delete book;
            }
        }

        return true;
    }

    bool saveBooks(const std::string& path,
                   const HashMap<std::string, Book*>& books)
    {
        std::ofstream file(path);

        if (!file.is_open())
            return false;

        for (auto it = books.begin();
             it != books.end();
             ++it)
        {
            const Book* book = it->value;

            file << book->getId() << '|'
                 << book->title << '|'
                 << book->author << '|'
                 << book->genre << '|'
                 << bookStatusToString(book->status) << '|'
                 << book->totalQuantity << '|'
                 << book->availableQuantity << '|'
                 << book->borrowCount
                 << '\n';
        }

        file.flush();
        return true;
    }

    // ==========================================================
    // READER
    // ==========================================================

    static bool parseReaderLine(const std::string& line,
                                Reader*& reader)
    {
        std::istringstream iss(line);

        std::string id;
        std::string name;
        std::string typeStr;
        std::string debtStr;

        if (!std::getline(iss, id, '|'))
            return false;

        if (!std::getline(iss, name, '|'))
            return false;

        if (!std::getline(iss, typeStr, '|'))
            return false;

        if (!std::getline(iss, debtStr))
            return false;

        try
        {
            long long debt = std::stoll(debtStr);

            reader = new Reader(
                id,
                name,
                stringToReaderType(typeStr),
                debt);

            return true;
        }
        catch (...)
        {
            return false;
        }
    }

    bool loadReaders(const std::string& path,
                     HashMap<std::string, Reader*>& readers)
    {
        std::ifstream file(path);

        if (!file.is_open())
        {
            std::ofstream create(path);
            create.close();
            return true;
        }

        std::string line;

        while (std::getline(file, line))
        {
            if (line.empty())
                continue;

            Reader* reader = nullptr;

            if (!parseReaderLine(line, reader))
            {
                delete reader;
                continue;
            }

            if (!readers.add(reader->getId(), reader))
            {
                delete reader;
            }
        }

        return true;
    }

    bool saveReaders(const std::string& path,
                     const HashMap<std::string, Reader*>& readers)
    {
        std::ofstream file(path);

        if (!file.is_open())
            return false;

        for (auto it = readers.begin(); it != readers.end(); ++it)
        {
            const Reader* reader = it->value;
            file << reader->getId() << '|' \
                 << reader->name << '|' \
                 << readerTypeToString(reader->type) << '|' \
                 << reader->debt << '\n';
        }

        file.flush();
        return true;
    }

    static bool parseBorrowRecordLine(const std::string& line,
                                      HashMap<std::string, Reader*>& readers,
                                      const HashMap<std::string, Book*>& books)
    {
        std::istringstream iss(line);
        std::string recordId;
        std::string readerId;
        std::string bookId;
        std::string borrowTimeStr;
        std::string returnTimeStr;

        if (!std::getline(iss, recordId, '|'))
            return false;
        if (!std::getline(iss, readerId, '|'))
            return false;
        if (!std::getline(iss, bookId, '|'))
            return false;
        if (!std::getline(iss, borrowTimeStr, '|'))
            return false;
        if (!std::getline(iss, returnTimeStr))
            return false;

        try
        {
            time_t borrowTime = static_cast<time_t>(std::stoll(borrowTimeStr));
            time_t returnTime = static_cast<time_t>(std::stoll(returnTimeStr));

            auto readerIt = readers.find(readerId);
            if (readerIt == readers.end())
                return false;
            Reader* reader = readerIt->value;

            auto bookIt = books.find(bookId);
            if (bookIt == books.end())
                return false;

            BorrowRecord* record = new BorrowRecord(recordId, bookId, borrowTime, returnTime);
            reader->borrowList.insertAtHead(record);
            return true;
        }
        catch (...)
        {
            return false;
        }
    }

    bool loadBorrowRecords(const std::string& path,
                           HashMap<std::string, Reader*>& readers,
                           const HashMap<std::string, Book*>& books)
    {
        std::ifstream file(path);

        if (!file.is_open())
        {
            std::ofstream create(path);
            create.close();
            return true;
        }

        std::string line;

        while (std::getline(file, line))
        {
            if (line.empty())
                continue;

            if (!parseBorrowRecordLine(line, readers, books))
                continue;
        }

        return true;
    }

    bool saveBorrowRecords(const std::string& path,
                           const HashMap<std::string, Reader*>& readers)
    {
        std::ofstream file(path);

        if (!file.is_open())
            return false;

        for (auto it = readers.begin(); it != readers.end(); ++it)
        {
            const Reader* reader = it->value;
            Node<BorrowRecord*>* current = reader->borrowList.getHead();
            while (current != nullptr)
            {
                BorrowRecord* record = current->data;
                file << record->recordId << '|' \
                     << reader->getId() << '|' \
                     << record->bookId << '|' \
                     << record->borrowDate << '|' \
                     << record->dueDate << '\n';
                current = current->next;
            }
        }

        file.flush();
        return true;
    }
}
