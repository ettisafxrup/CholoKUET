#pragma once

#include <string>
#include <vector>

#include "Location.h"

std::vector<int> linearSearchByName(const std::vector<Location> &locations,
                                    const std::string &query, int &comparisons);

std::vector<int> linearSearchByCategory(const std::vector<Location> &locations,
                                        const std::string &query, int &comparisons);

int linearSearchById(const std::vector<Location> &locations, int id, int &comparisons);

bool isSortedByName(const std::vector<Location> &locations);

int binarySearchByName(const std::vector<Location> &sorted, const std::string &name,
                       int &comparisons);
