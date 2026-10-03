# Where each data structure and algorithm is used

| Marking area | What we built | Where it lives | What it does in the app |
|---|---|---|---|
| **Array** | `std::vector<Location>` | `Campus::locations` | The list of every campus location |
| **Linked list** | `std::list<int>` | `Navigator::recentSearches`, `favorites`, BFS result | Recent searches (newest first, max 10), favorites, and the route |
| **Stack** | Our own array stack | `dsa/stack/` | DFS, the Back button in Trip history, and turning a BFS route the right way round |
| **Queue** | Our own circular queue | `dsa/queue/` | BFS in `Graph::findRoute` |
| **Tree** | General tree, children in a vector | `CampusTree` | Browse by category: KUET → category → place |
| **Graph** | Adjacency list | `Graph` | Places are vertices, walkways are edges with a length in meters |
| **BFS** | Our own, on our Queue | `Graph::findRoute` | Take me somewhere: the route with the fewest stops, then broken down by road in `Trip.cpp` |
| **DFS** | Our own, on our Stack | `Graph::depthFirstOrder` | Explore the campus and the connectivity check |
| **Searching** | Linear and binary search | `Search.cpp` | Search by name, category or ID, and exact-name lookup |
| **Sorting** | Bubble and selection sort | `Sort.cpp` | Sort places, and ordering "What's near me" by distance |

Stack and Queue are written from scratch because those are the structures the lab asked us to build by hand. The other containers come from the STL, as the lab allows. The algorithms on top of them (BFS, DFS, both searches, both sorts) are all our own code. `make check-manual` confirms that `std::stack`, `std::queue` and `std::sort` appear nowhere in the code.

## Why each one fits

- **Array** – places are looked up by position all the time, and the list rarely changes.
- **Linked list** – recent searches keep moving an item to the front and dropping the oldest from the back. A route's length isn't known until BFS finishes.
- **Stack** – "Back" means going to the last place you visited (last in, first out). DFS needs the most recent unfinished place, which is also a stack.
- **Queue** – BFS has to finish every place one step away before any place two steps away (first in, first out).
- **Tree** – KUET → Academic → CSE is a natural hierarchy. Each category can hold any number of places, so we use a general tree rather than a binary one.
- **Graph** – walkways connect places in loops and crossings, which a tree can't represent.

## Complexity

| Operation | Time |
|---|---|
| Array access by index | O(1) |
| Find a location by ID | O(n) |
| Linked list insert at front | O(1) |
| Stack push / pop / peek | O(1) |
| Queue enqueue / dequeue | O(1) |
| Tree traversal | O(n) |
| BFS / DFS (adjacency list) | O(V + E) |
| Linear search | O(n) |
| Binary search (sorted data) | O(log n) |
| Bubble sort | O(n²), O(n) if already sorted |
| Selection sort | O(n²) |
| Nearby places (distances + bubble sort) | O(n²) |

## BFS gives the fewest stops, not always the shortest walk

BFS treats every walkway as one step. It finds the route with the fewest stops, but walkways have different lengths, so that isn't always the shortest walk in meters. The app shows both the walking length and the straight-line distance, so the difference is easy to see. Dijkstra's algorithm would give the shortest walk in meters; we list it as future work.
