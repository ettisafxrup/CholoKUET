#pragma once

#include <iostream>

namespace style
{
    void init();
    bool enabled();

    std::ostream &accent(std::ostream &os);
    std::ostream &heading(std::ostream &os);
    std::ostream &strong(std::ostream &os);
    std::ostream &muted(std::ostream &os);
    std::ostream &good(std::ostream &os);
    std::ostream &warn(std::ostream &os);
    std::ostream &bad(std::ostream &os);
    std::ostream &reset(std::ostream &os);

    void clearScreen();
}
