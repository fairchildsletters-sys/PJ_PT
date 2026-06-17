#ifndef MATH_UTILS_H
#define MATH_UTILS_H

#include <cstddef>

namespace MathUtils
{
    // Check if a number is prime
    inline bool isPrime(size_t n)
    {
        if (n <= 1)
            return false;
        if (n <= 3)
            return true;
        if (n % 2 == 0 || n % 3 == 0)
            return false;
        for (size_t i = 5; i * i <= n; i += 6)
        {
            if (n % i == 0 || n % (i + 2) == 0)
                return false;
        }
        return true;
    }

    // Find the next prime number >= n
    inline size_t getNextPrime(size_t n)
    {
        if (n <= 2)
            return 2;
        if (n % 2 == 0)
            n++;

        while (!isPrime(n))
        {
            n += 2;
        }
        return n;
    }

} // namespace MathUtils

#endif // MATH_UTILS_H