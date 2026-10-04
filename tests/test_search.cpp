#include "../include/Search.h"
#include "../include/Sort.h"
#include "check.h"

int main()
{
    std::vector<Location> places = {
        {1, "KUET Main Gate", "Transportation", "", 0, 0},
        {3, "Central Library", "Facility", "", 0, 0},
        {4, "Department of CSE", "Academic", "", 0, 0},
        {14, "Department Library", "Academic", "", 0, 0},
        {18, "KUET Auditorium", "Facility", "", 0, 0},
    };
    int comparisons = 0;

    std::vector<int> found = linearSearchByName(places, "LIB", comparisons);
    CHECK(found.size() == 2);
    CHECK(found[0] == 1 && found[1] == 3);
    CHECK(comparisons == 5);

    CHECK(linearSearchByName(places, "Swimming Pool", comparisons).empty());

    CHECK(linearSearchByCategory(places, "academic", comparisons).size() == 2);
    CHECK(linearSearchById(places, 14, comparisons) == 3 && comparisons == 4);
    CHECK(linearSearchById(places, 99, comparisons) == -1);

    std::vector<Location> sorted = places;
    CHECK(!isSortedByName(sorted));
    sortByName(sorted);
    CHECK(isSortedByName(sorted));

    for (size_t i = 0; i < sorted.size(); i++)
    {
        CHECK(binarySearchByName(sorted, sorted[i].name, comparisons) == static_cast<int>(i));
        CHECK(comparisons <= 3);
    }
    CHECK(binarySearchByName(sorted, "central library", comparisons) != -1);
    CHECK(binarySearchByName(sorted, "Zoo", comparisons) == -1);
    CHECK(binarySearchByName({}, "Anything", comparisons) == -1 && comparisons == 0);

    return finish("Search");
}
