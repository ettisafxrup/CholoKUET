#pragma once

#include <functional>
#include <string>

#include "../../include/Config.h"
class Stack
{
public:
    static const int CAPACITY = MAX_LOCATIONS;

    bool push(int value);
    bool pop(int &value);
    bool peek(int &value) const;

    bool isEmpty() const { return top == -1; }
    bool isFull() const { return top == CAPACITY - 1; }
    int size() const { return top + 1; }
    void clear() { top = -1; }

    int at(int i) const { return data[i]; }

    void print(const std::function<std::string(int)> &label = nullptr) const;

    void printInline(const std::function<std::string(int)> &label = nullptr) const;

private:
    int data[CAPACITY];
    int top = -1;
};
