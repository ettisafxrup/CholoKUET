#include "Stack.h"

#include <iostream>

bool Stack::push(int value)
{
    if (isFull())
        return false;   // overflow

    data[++top] = value;
    return true;
}

bool Stack::pop(int& value)
{
    if (isEmpty())
        return false;   // underflow

    value = data[top--];
    return true;
}

bool Stack::peek(int& value) const
{
    if (isEmpty())
        return false;

    value = data[top];
    return true;
}

void Stack::print(const std::function<std::string(int)>& label) const
{
    if (isEmpty())
    {
        std::cout << "  (empty stack, top = -1)\n";
        return;
    }

    std::cout << "  TOP  (top = " << top << ")\n   ↓\n";
    for (int i = top; i >= 0; i--)
    {
        std::cout << "  " << (label ? label(data[i]) : std::to_string(data[i])) << "\n";
    }
}

void Stack::printInline(const std::function<std::string(int)>& label) const
{
    std::cout << "     stack: [bottom] ";
    for (int i = 0; i <= top; i++)
        std::cout << (label ? label(data[i]) : std::to_string(data[i])) << (i < top ? " | " : " ");
    std::cout << "[top]\n";
}
