#pragma once

#include <functional>
#include <string>

#include "../../include/Config.h"

// Array-based stack of integers (we store location IDs in it).
//
// `top` is the index of the element on top. An empty stack has top == -1,
// so push moves top up and pop moves it back down.
//
// Used for DFS, the "Back" navigation history, and for reversing a BFS
// route while rebuilding it.
class Stack
{
public:
    static const int CAPACITY = MAX_LOCATIONS;

    bool push(int value);
    bool pop(int& value);
    bool peek(int& value) const;

    bool isEmpty() const { return top == -1; }
    bool isFull() const { return top == CAPACITY - 1; }
    int size() const { return top + 1; }
    void clear() { top = -1; }

    // Element at position i counted from the bottom (0 = oldest).
    // Lets us list the history without popping it.
    int at(int i) const { return data[i]; }

    // Prints the stack top-first. `label` turns an ID into a name.
    void print(const std::function<std::string(int)>& label = nullptr) const;

    // One-line version for step-by-step traces: [bottom] a | b | c [top]
    void printInline(const std::function<std::string(int)>& label = nullptr) const;

private:
    int data[CAPACITY];
    int top = -1;
};
