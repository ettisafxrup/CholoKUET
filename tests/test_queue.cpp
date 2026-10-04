#include "../dsa/queue/Queue.h"
#include "check.h"

int main()
{
    Queue queue;
    int value = 0;

    CHECK(queue.isEmpty());
    CHECK(!queue.dequeue(value));
    CHECK(!queue.peek(value));

    queue.enqueue(10);
    queue.enqueue(20);
    queue.enqueue(30);
    CHECK(queue.peek(value) && value == 10);
    CHECK(queue.dequeue(value) && value == 10);
    CHECK(queue.at(0) == 20 && queue.at(1) == 30);
    CHECK(queue.size() == 2);

    queue.clear();
    for (int i = 0; i < Queue::CAPACITY; i++)
        CHECK(queue.enqueue(i));
    CHECK(queue.isFull());
    CHECK(!queue.enqueue(999));

    for (int i = 0; i < 3; i++)
        CHECK(queue.dequeue(value) && value == i);
    for (int i = 0; i < 3; i++)
        CHECK(queue.enqueue(1000 + i));
    CHECK(queue.isFull());
    CHECK(queue.rearIndex() == 2);
    CHECK(queue.frontIndex() == 3);

    for (int i = 3; i < Queue::CAPACITY; i++)
        CHECK(queue.dequeue(value) && value == i);
    for (int i = 0; i < 3; i++)
        CHECK(queue.dequeue(value) && value == 1000 + i);
    CHECK(queue.isEmpty());

    return finish("Queue");
}
