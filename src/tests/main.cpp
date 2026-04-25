#include <iostream>
#include <fstream>
#include <string>

#include "core/board.hpp"
#include "engine/perft.hpp"
#include "engine/evaluate.hpp"
#include "core/bitboard.hpp"

// console entrypoint for testing engine functionality

#include "../core/board.cpp"
#include "../core/fen.cpp"
#include "../core/makemove.cpp"
#include "../core/movegen.cpp"
#include "../engine/evaluate.cpp"
#include "../engine/perft.cpp"

const std::string KIWIPETE = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -";

void print_perft_result(const Board &board, int depth);

void perft_depth_5_starting_position();
void kiwipete_depth_4();
void eval();


// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//             MAIN
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

int main()
{
    
    return 0;
}




void print_perft_result(const Board &board, int depth)
{
    const std::uint64_t actual = perft(board, depth); // result from perft algorithm

    std::cout << "perft(" << depth << ") = " << actual << '\n';
}

void perft_depth_5_starting_position() // passed!
{
    Board board;
    load_start_position(board);

    std::cout << "Perft Depth : 5\n";

    for (int i = 1; i <= 5; i++)
    {
        print_perft_result(board, i);
    }
}

void kiwipete_depth_4() // passed!
{
    Board board;
    load_start_position(board);
    load_fen(board, KIWIPETE);
    for (int i = 1; i <= 4; i++)
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


