#ifndef HASHER_H
#define HASHER_H

#include <string>
#include <cstddef>

// General Template for Hasher
// Generates a compile-time error if an undefined key type is used
template <typename T>
struct Hasher;

// Hasher for std::string
template <>
struct Hasher<std::string>
{
    // DJB2 Hashing
    static inline size_t hash(const std::string &key)
    {
        size_t hashVal = 5381;
        for (char c : key)
        {
            hashVal = ((hashVal << 5) + hashVal) + c; // hash * 33 + c
        }
        return hashVal; // only hash value
    }
};

#endif // HASHER_H