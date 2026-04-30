#include <iostream>
#include <fstream>
#include <string>

#include "core/board.hpp"
#include "engine/perft.hpp"
#include "engine/evaluate.hpp"
#include "engine/tt.hpp"
#include "engine/zobrist.hpp"
#include "core/bitboard.hpp"

// console entrypoint for testing engine functionality

#include "../core/board.cpp"
#include "../core/fen.cpp"
#include "../core/makemove.cpp"
#include "../core/movegen.cpp"
#include "../engine/evaluate.cpp"
#include "../engine/zobrist.cpp"
#include "../engine/perft.cpp"
#include "../core/bitboard.cpp"
#include "../tests/version_csv_writer.cpp"
#include "../tests/perft_debug.cpp"

const std::string KIWIPETE = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -";

void print_perft_result(const Board &board, int depth);
void perft_depth_n_starting_position(int depth);
void kiwipete_depth_n(int depth);
void eval_to_csv();
void engine_tests(string fen, int depth);


// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//             MAIN
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

int main()
{
    init_zobrist();
    tt_init(1 << 20);
    run_perft_debug(KIWIPETE, 4);
    return 0;
}

/*
    UNIT TESTING:
    avoid illegal en passant capture:
        8/5bk1/8/2Pp4/8/1K6/8/8 w - d6 0 1          perft 6 = 824064 PASS
        8/8/1k6/8/2pP4/8/5BK1/8 b - d3 0 1          perft 6 = 824064 PASS
    en passant capture checks opponent:
        8/8/1k6/2b5/2pP4/8/5K2/8 b - d3 0 1         perft 6 = 1440467 FAIL Nodes: 1440542, Captures: 38917
        8/5k2/8/2Pp4/2B5/1K6/8/8 w - d6 0 1         perft 6 = 1440467 FAIL Nodes: 1440542, Captures: 38917
    short castling gives check:
        5k2/8/8/8/8/8/8/4K2R w K - 0 1              perft 6 = 661072 PASS
        4k2r/8/8/8/8/8/8/5K2 b k - 0 1              perft 6 = 661072 PASS
    long castling gives check:
        3k4/8/8/8/8/8/8/R3K3 w Q - 0 1              perft 6 = 803711 PASS
        r3k3/8/8/8/8/8/8/3K4 b q - 0 1              perft 6 = 803711 PASS
    castling (including losing cr due to rook capture):
        r3k2r/1b4bq/8/8/8/8/7B/R3K2R w KQkq - 0 1   perft 4 = 1274206 PASS
        r3k2r/7b/8/8/8/8/1B4BQ/R3K2R b KQkq - 0 1   perft 4 = 1274206 PASS
    castling prevented:
        r3k2r/8/3Q4/8/8/5q2/8/R3K2R b KQkq - 0 1    perft 4 = 1720476 PASS
        r3k2r/8/5Q2/8/8/3q4/8/R3K2R w KQkq - 0 1    perft 4 = 1720476
    promote out of check:
        2K2r2/4P3/8/8/8/8/8/3k4 w - - 0 1           perft 6 = 3821001 PASS
        3K4/8/8/8/8/8/4p3/2k2R2 b - - 0 1           perft 6 = 3821001
    discovered check:
        8/8/1P2K3/8/2n5/1q6/8/5k2 b - - 0 1         perft 5 = 1004658 PASS
        5K2/8/1Q6/2N5/8/1p2k3/8/8 w - - 0 1         perft 5 = 1004658
    promote to give check:
        4k3/1P6/8/8/8/8/K7/8 w - - 0 1              perft 6 = 217342 PASS
        8/k7/8/8/8/8/1p6/4K3 b - - 0 1              perft 6 = 217342
    underpromote to check:
        8/P1k5/K7/8/8/8/8/8 w - - 0 1               perft 6 = 92683 PASS
        8/8/8/8/8/k7/p1K5/8 b - - 0 1               perft 6 = 92683
    self stalemate:
        K1k5/8/P7/8/8/8/8/8 w - - 0 1               perft 6 = 2217 PASS
        8/8/8/8/8/p7/8/k1K5 b - - 0 1               perft 6 = 2217
    stalemate/checkmate:
        8/k1P5/8/1K6/8/8/8/8 w - - 0 1              perft 7 = 567584 PASS
        8/8/8/8/1k6/8/K1p5/8 b - - 0 1              perft 7 = 567584
    double check:
        8/8/2k5/5q2/5n2/8/5K2/8 b - - 0 1           perft 4 = 23527 PASS
        8/5k2/8/5N2/5Q2/2K5/8/8 w - - 0 1           perft 4 = 23527
*/



void print_perft_result(const Board &board, int depth)
{
    const std::uint64_t actual = perft(board, depth); // result from perft algorithm

    std::cout << "perft(" << depth << ") = " << actual << '\n';
}

void perft_depth_n_starting_position(int depth) // passed!
{
    Board board;
    load_start_position(board);

    std::cout << "Perft Depth : "<< depth << '\n';

    for (int i = 1; i <= depth; i++)
    {
        print_perft_result(board, i);
    }
}

void kiwipete_depth_n(int depth) // passed!
{
    Board board;
    load_fen(board, KIWIPETE);
    for (int i = 1; i <= depth; i++)
    {
        print_perft_result(board, i);
    }
}

void eval_to_csv()
{
    if (evaluate_csv_positions())
    {
        std::cout << "Wrote eval results to src/tests/output/Evals.csv\n";
        return;
    }

    std::cout << "Failed to write eval results to src/tests/output/Evals.csv\n";
}



void engine_tests(string fen, int depth)
{
    Board board;
    load_fen(board, fen);

    std::cout << "Perft Depth : "<< depth << '\n';
    std::cout << "FEN : "<< fen << '\n';

    for (int i = 1; i <= depth; i++)
    {
        print_perft_result(board, i);
    }
}
