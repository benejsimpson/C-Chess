#include "gui.hpp"

int main()
{
    ChessGui app;

    if (!app.init())
        return 1;

    app.run();
    return 0;
}