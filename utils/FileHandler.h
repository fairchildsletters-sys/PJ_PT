#ifndef FILE_HANDLER_H
#define FILE_HANDLER_H

#include <string>
#include "../datastructures/HashMap.h"
#include "../models/Book.h"
#include "../models/Reader.h"

namespace FileHandler
{
    bool loadBooks(const std::string &path, HashMap<std::string, Book *> &books);
    bool saveBooks(const std::string &path, const HashMap<std::string, Book *> &books);

    bool loadReaders(const std::string &path, HashMap<std::string, Reader *> &readers);
    bool saveReaders(const std::string &path, const HashMap<std::string, Reader *> &readers);

    bool loadBorrowRecords(const std::string &path, HashMap<std::string, Reader *> &readers, const HashMap<std::string, Book *> &books);
    bool saveBorrowRecords(const std::string &path, const HashMap<std::string, Reader *> &readers);
}

#endif // FILE_HANDLER_H
