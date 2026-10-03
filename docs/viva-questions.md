# Viva practice questions

Short answers in our own words, based on how CholoKUET actually works.

## Array

1. **Why store locations in an array?** Places are read far more often than they change, and an array gives O(1) access by index.
2. **Access cost?** O(1). Finding a place by its ID is O(n), because we have to scan.
3. **What happens when it's full?** We cap it at `MAX_LOCATIONS` (200), and adding a place after that is refused with a message.
4. **Why not assume ID = index?** Deleting a place shifts the ones after it, so the indexes change. IDs never change, so everything refers to places by ID.
5. **Cost of deleting from the middle?** O(n), because the later items shift left.

## Linked list

6. **Where do we use one?** Recent searches, favorites, and the route that BFS returns.
7. **Why a list for recent searches?** We keep adding at the front and dropping the oldest from the back. Both are O(1) in a linked list.
8. **Array vs linked list?** An array gives fast access by index. A list is cheap to insert into and remove from at the ends, and it grows as needed.
9. **Why is a route a list?** We don't know how many stops there are until BFS finishes, and we only ever walk it from start to end.

## Stack

10. **What is LIFO?** Last in, first out: the last item pushed is the first one popped.
11. **Show `top` for push 10, 20, 30.** It goes -1 → 0 → 1 → 2. Popping returns 30 and `top` becomes 1.
12. **What is stack overflow here?** Pushing when `top == CAPACITY - 1`. `push` returns false instead of writing past the array.
13. **Underflow?** Popping when `top == -1`. `pop` returns false.
14. **Where do we use the stack?** DFS, the Back button, and reversing the BFS route.
15. **Why does Back need a stack?** Going back means returning to the most recent place, which is exactly LIFO.
16. **Why not `std::stack`?** The lab asked us to build Stack and Queue ourselves, and `make check-manual` confirms we didn't use the STL ones.

## Queue

17. **What is FIFO?** First in, first out.
18. **Why a circular queue?** In a plain array queue, slots freed at the front are never used again. Wrapping the indexes with `% CAPACITY` reuses them.
19. **How do you tell full from empty?** We keep a `count`. Using only `front` and `rear`, the two cases look the same.
20. **Why does BFS need a queue?** It must finish every place one step away before any place two steps away, and FIFO order does exactly that.
21. **Can BFS overflow our queue?** No. Each place is queued at most once, and the graph can't have more than 200 places.

## Tree

22. **Why a tree for categories?** KUET → category → place is a natural hierarchy with one parent per node.
23. **Why a general tree and not a binary tree?** A category can hold any number of places.
24. **Name the parent and child of "Academic".** The parent is KUET. The children are the academic buildings.
25. **What is preorder traversal?** Visit the node first, then each child subtree from left to right.
26. **Height of our campus tree?** 3: root, categories, places.
27. **How is memory freed?** Children are stored by value inside their parent, so removing a node removes its whole subtree automatically.

## Graph

28. **Vertex and edge in our app?** A vertex is a place and an edge is a walkway, with its length in meters.
29. **Directed or undirected?** Undirected, because you can walk a path both ways. `addEdge` stores it in both directions.
30. **Adjacency matrix or list?** A list: our campus is sparse (44 places, 56 paths), and BFS/DFS then cost O(V + E) instead of O(V²).
31. **How are edge lengths found?** From the data file, or with the Haversine distance if the file leaves them out.

## BFS

32. **Why BFS for routes?** In an unweighted graph it finds the route with the fewest stops.
33. **Why mark a place visited when it's enqueued?** Otherwise the same place can be queued several times.
34. **What is `parent[]` for?** It records how we reached each place, so the route can be rebuilt afterwards.
35. **How is the route rebuilt?** Follow `parent` back from the goal, push each place onto our stack, then pop them off to get the route in forward order.
36. **Is BFS the shortest walk in meters?** Not always. It minimises stops, not distance. Dijkstra would find the shortest walk in meters.
37. **Complexity?** O(V + E).
38. **What if there's no route?** The queue empties before we reach the goal, and the app says so.

## DFS

39. **Why a stack for DFS?** It always continues from the most recent place that still has unvisited neighbours.
40. **Recursive or iterative?** Iterative. Our stack does what the call stack would do in the recursive version, and `nextEdge` remembers where each place's neighbour scan stopped.
41. **Why can't it overflow our stack?** Each place is pushed only once, so the stack never holds more than V ≤ 200 items.
42. **How is connectivity checked?** Run DFS from any place and compare the number reached with the total.
43. **Complexity?** O(V + E).

## Searching

44. **Linear vs binary search?** Linear is O(n) and works on any order; binary is O(log n) but only on sorted data.
45. **Why does binary search need sorted data?** Every comparison throws away half the list, and that's only valid if everything on one side is smaller.
46. **How is search case-insensitive?** Both strings are lowercased before comparing, so "lib" matches "Central Library".

## Sorting

47. **How does bubble sort work?** It swaps neighbours that are out of order. The largest remaining item ends up at the end each pass, and it stops early if a pass makes no swaps.
48. **How does selection sort work?** Each pass finds the smallest remaining item and swaps it into place.
49. **Which is stable?** Bubble sort, because equal items never swap. Selection sort can jump an item over an equal one.
50. **Complexities?** Both are O(n²). Bubble sort is O(n) on data that's already sorted.
51. **Why count comparisons and swaps?** To show the difference between the two algorithms: selection makes at most n−1 swaps, bubble often many more.

## Application

52. **How is distance computed?** With the Haversine formula on decimal-degree coordinates, using an Earth radius of 6,371 km.
53. **How does "Where am I?" work?** It works out the distance from the given point to every place, sorts them, and picks the closest.
54. **Where do the coordinates come from?** OpenStreetMap for most places. The rest come from the KUET master plan, lined up with OSM (27 m average error).
55. **Is the login secure?** It's fine for a demo but not real security. Passwords are stored as an FNV-1a hash, which is fast to brute-force; a real system would use bcrypt or Argon2.
