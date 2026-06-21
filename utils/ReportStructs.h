#ifndef REPORT_STRUCTS_H
#define REPORT_STRUCTS_H

#include <string>

struct BookReport
{
    std::string bookId;
    std::string title;
    std::string author;
    std::string genre;
    int borrowCount;
};

#endif