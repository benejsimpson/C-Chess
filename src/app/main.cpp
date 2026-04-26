#include "core/board.hpp"
#include "core/bitboard.hpp"
#include "core/fen.hpp"
#include "core/makemove.hpp"
#include "core/move.hpp"
#include "core/movegen.hpp"
#include "gui/constants.hpp"
#include "gui/game.hpp"
#include "gui/gui.hpp"
#include "gui/gui_state.hpp"
#include "gui/input.hpp"
#include "gui/renderer.hpp"
#include "gui/textures.hpp"
#include "gui/widgets.hpp"
#include "engine/ai.hpp"

#include "../core/board.cpp"
#include "../core/fen.cpp"
#include "../core/makemove.cpp"
#include "../core/movegen.cpp"
#include "../gui/game.cpp"
#include "../gui/input.cpp"
#include "../gui/renderer.cpp"
#include "../gui/textures.cpp"
#include "../gui/gui.cpp"
#include "../core/bitboard.cpp"
#include "../engine/ai.cpp"
#include "../engine/evaluate.cpp"


int main()
{
    ChessGui app;

    if (!app.init())
        return 1;

    app.run();
    return 0;
}
