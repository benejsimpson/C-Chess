NOTES: 

TO DO:
- improve side attack mask updating
    - pawns, kings, knights dont need updating unless they move
- magic bitboards?


movegen.cpp
OLD : moves.push_back(createMove(from, to, piece, captured))
NEW : moves.push_back(make_move(from, to))

SPECIAL MOVES:
    moves.push_back(make_move(from, to, DOUBLE_PAWN))
    moves.push_back(make_move(from, to, EN_PASSANT))
    moves.push_back(make_move(from, to, KING_CASTLE))
    moves.push_back(make_move(from, to, QUEEN_CASTLE))

PROMO:
    moves.push_back(make_move(from, to, Q_PROMO))
    moves.push_back(make_move(from, to, R_PROMO))
    moves.push_back(make_move(from, to, B_PROMO))
    moves.push_back(make_move(from, to, N_PROMO))


makemove.cpp
void makeMove(Board& board, Move move)
{
    const int from = moveFrom(move);
    const int to = moveTo(move);
    const MoveFlag flag = moveFlag(move);

    const Piece moving_piece = board.squares[from];

    board.squares[from] = Empty;

    if (flag == EN_PASSANT)
    {
        const int captured_pawn_square = board.whiteToMove ? to - 8 : to + 8;
        board.squares[captured_pawn_square] = Empty;
    }

    if (flag == KING_CASTLE)
    {
        if (moving_piece == WK)
        {
            board.squares[6] = WK;
            board.squares[5] = WR;
            board.squares[7] = Empty;
        }
        else
        {
            board.squares[62] = BK;
            board.squares[61] = BR;
            board.squares[63] = Empty;
        }
    }
    else if (flag == QUEEN_CASTLE)
    {
        if (moving_piece == WK)
        {
            board.squares[2] = WK;
            board.squares[3] = WR;
            board.squares[0] = Empty;
        }
        else
        {
            board.squares[58] = BK;
            board.squares[59] = BR;
            board.squares[56] = Empty;
        }
    }
    else if (is_promotion(move))
    {
        board.squares[to] = promoted_piece(flag, board.whiteToMove);
    }
    else
    {
        board.squares[to] = moving_piece;
    }

    board.whiteToMove = !board.whiteToMove;
}
