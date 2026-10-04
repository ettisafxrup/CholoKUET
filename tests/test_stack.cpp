#include "../dsa/stack/Stack.h"
#include "check.h"

int main()
{
    Stack stack;
    int value = 0;

    CHECK(stack.isEmpty());
    CHECK(stack.size() == 0);
    CHECK(!stack.pop(value));
    CHECK(!stack.peek(value));

    stack.push(10);
    stack.push(20);
    stack.push(30);
    CHECK(stack.size() == 3);
    CHECK(stack.peek(value) && value == 30);
    CHECK(stack.pop(value) && value == 30);
    CHECK(stack.pop(value) && value == 20);
    CHECK(stack.at(0) == 10);

    stack.clear();
    for (int i = 0; i < Stack::CAPACITY; i++)
        CHECK(stack.push(i));
    CHECK(stack.isFull());
    CHECK(!stack.push(999));
    CHECK(stack.pop(value) && value == Stack::CAPACITY - 1);

    stack.clear();
    CHECK(!stack.pop(value));

    return finish("Stack");
}
