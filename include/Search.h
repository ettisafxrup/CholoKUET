#pragma once

#include <string>
#include <vector>

#include "Location.h"

// Linear search checks items one by one: O(n), works on any order, and can
// do partial, case-insensitive matches ("lib" finds "Central Library").
//
// Binary search halves the range each step: O(log n), but ONLY on a list
// already sorted by name. Sort first with sortByName() from Sort.h.
//
// Each function reports how many comparisons it made.

// Indexes of every location whose name contains `query`.
std::vector<int> linearSearchByName(const std::vector<Location>& locations,
                                    const std::string& query, int& comparisons);

// Indexes of every location whose category contains `query`.
std::vector<int> linearSearchByCategory(const std::vector<Location>& locations,
                                        const std::string& query, int& comparisons);

// Index of the location with this ID, or -1.
int linearSearchById(const std::vector<Location>& locations, int id, int& comparisons);

bool isSortedByName(const std::vector<Location>& locations);

// Exact (case-insensitive) name lookup. Returns the index or -1.
int binarySearchByName(const std::vector<Location>& sorted, const std::string& name,
                       int& comparisons);
