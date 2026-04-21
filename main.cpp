#include "board.hpp"
#include "fen.hpp"
#include "ui.hpp"
#include "utils.h"

using namespace std;

int main() {
    Board board;

    // load starting position (FEN)
    string fen;
    load_fen(board, fen);

    // initialise UI
    init_window();
    run_ui(board);

    // run UI loop

    return 0;
}