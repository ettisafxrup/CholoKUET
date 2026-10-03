#pragma once

#include <functional>
#include <string>

#include "../../include/Config.h"

// Circular array queue of integers (location IDs).
//
// `front` is where the next dequeue happens, `rear` is the last slot we
// wrote to, and `count` tells an empty queue apart from a full one.
// Both ends wrap around with % CAPACITY, so slots freed by dequeue are
// reused instead of the queue "walking off" the end of the array.
//
// Used by BFS when finding routes.
class Queue
{
public:
    static const int CAPACITY = MAX_LOCATIONS;

    bool enqueue(int value);
    bool dequeue(int& value);
    bool peek(int& value) const;

    bool isEmpty() const { return count == 0; }
    bool isFull() const { return count == CAPACITY; }
    int size() const { return count; }
    void clear();

    // i-th element counted from the front (0 = next to leave).
    int at(int i) const { return data[(front + i) % CAPACITY]; }

    int frontIndex() const { return front; }
    int rearIndex() const { return rear; }

    // Prints FRONT → a b c ← REAR. `label` turns an ID into a name.
    void print(const std::function<std::string(int)>& label = nullptr) const;

private:
    int data[CAPACITY];
    int front = 0;
    int rear = -1;
    int count = 0;
};
