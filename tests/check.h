#pragma once

// A tiny test helper: CHECK(expr) counts a pass or prints the failing line.

#include <iostream>

inline int passed = 0;
inline int failed = 0;

#define CHECK(expr)                                                         \
    do                                                                      \
    {                                                                       \
        if (expr)                                                           \
            passed++;                                                       \
        else                                                                \
        {                                                                   \
            failed++;                                                       \
            std::cout << "  FAILED line " << __LINE__ << ": " #expr "\n";   \
        }                                                                   \
    } while (0)

inline int finish(const char* suite)
{
    std::cout << suite << ": " << passed << " passed, " << failed << " failed\n";
    return failed == 0 ? 0 : 1;
}
