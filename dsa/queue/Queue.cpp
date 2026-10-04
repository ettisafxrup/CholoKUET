#include "Queue.h"

#include <iostream>

bool Queue::enqueue(int value)
{
    if (isFull())
        return false;

    rear = (rear + 1) % CAPACITY;
    data[rear] = value;
    count++;
    return true;
}

bool Queue::dequeue(int &value)
{
    if (isEmpty())
        return false;

    value = data[front];
    front = (front + 1) % CAPACITY;
    count--;
    return true;
}

bool Queue::peek(int &value) const
{
    if (isEmpty())
        return false;

    value = data[front];
    return true;
}

void Queue::clear()
{
    front = 0;
    rear = -1;
    count = 0;
}

void Queue::print(const std::function<std::string(int)> &label) const
{
    std::cout << "  FRONT → ";
    if (isEmpty())
        std::cout << "(empty) ";

    for (int i = 0; i < count; i++)
    {
        std::cout << (label ? label(at(i)) : std::to_string(at(i)));
        std::cout << (i + 1 < count ? (label ? ", " : " ") : " ");
    }
    std::cout << "← REAR\n";
}
