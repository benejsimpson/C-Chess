#pragma once

#include "core/utils.hpp"
#include "board.hpp"
#include "move.hpp"
#include "core/movelist.hpp"
#include "core/attackMasks.hpp"
#include <array>
#include <cstdint>

using BitB = uint64_t;

//--------------------- Attack Masks

inline BitB generateAttackMasksForSide(const Board &board, bool white);

inline void updateAttackMasks(Board &board);

//--------------------- Passed Pawn Masks

// returns true if a pawn on given square is a passed pawn (no enemy pawns in front or on adjacent files)
inline bool isPassedPawn(const Board &board, int square, bool white);

//--------------------- Move Generation

// generates all possible moves that can be made by a side
MoveList generatePseudoLegalMoves(const Board &board);

// generates only legal moves that can be made - not in check after
MoveList generateLegalMoves(const Board &board);

// legal move generation for a specific piece on a square
MoveList generateLegalMovesForSquare(const Board &board, const int square);

//--------------------- Move Legality & Check Detection

inline bool isInCheck(const Board &board, bool white_king);

inline bool isCheckmate(Board &board);

static void generateKingCastles(const Board &board, MoveList &moves, int from);

inline bool sameMove(const Move &a, const Move &b);

//--------------------- Move scoring helpers
