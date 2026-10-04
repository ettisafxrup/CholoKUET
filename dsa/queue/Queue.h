#pragma once

#include <functional>
#include <string>

#include "../../include/Config.h"

class Queue
{
public:
    static const int CAPACITY = MAX_LOCATIONS;

    bool enqueue(int value);
    bool dequeue(int &value);
    bool peek(int &value) const;

    bool isEmpty() const { return count == 0; }
    bool isFull() const { return count == CAPACITY; }
    int size() const { return count; }
    void clear();

    int at(int i) const { return data[(front + i) % CAPACITY]; }

    int frontIndex() const { return front; }
    int rearIndex() const { return rear; }

    void print(const std::function<std::string(int)> &label = nullptr) const;

private:
    int data[CAPACITY];
    int front = 0;
    int rear = -1;
    int count = 0;
};
