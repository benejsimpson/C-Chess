# TO DO:

## Bitboards
Add:
- `BitBoard white_occup`
- `BitBoard black_occup`
- `BitBoard all_occup`
by ORing each pieces bitboard

### Structure for piece / board lookup
_from white's pov_

what piece on sq? currently using `board.squares[sq]`
- all_occup & sq == 0 -> sq empty; return;
- white_occup & sq == 0 -> black on sq;

position of all WP = `bb.WP` (ULL)

check sq has WP -> `WP & set_bit(sq)` == WP is on sq



### Bitwise Operators
_using A,B,C as ULL and A.x as the bit of A at position x_

__AND__ : & 
- sets `C.x` to 1 iff `A.x AND B.x = 1`

__OR__ : | 
- sets `C.x` to 1 iff `A.x OR B.x = 1`

__NOT__ : ~ A
- `C` is inverse of `A` : all 1 -> 0, 0 -> 1

__XOR__ : A ^ B 
- sets `C.x` to 1 if `A.x != B.x`
- use this to unset a bit with `A ^ set_bit(x)`

__Left shift__ : A << x 
- shifts A _x_ times to the left
- `set_bit(x)` : _0ULL | (1 << x)_ is all 0s other than a 1 at position _x_

__Right shift__ : _same as left shift but other direction_

# ENGINE TIME WOOOHOOO!
- perft to test move logic and rules - DONE!

## Evaluate positions
`evaluate(board)`
for each white piece on board:
    `eval` += (`piece_val` + `PSqT.piece(pos)`)
for each black piece on board:
    `eval` -= (`piece_val` + `PSqT.piece(pos)`)

## Piece Square Tables
`PSqT`
_For each piece type, assign a +/- value relative to the strength of the piece in that position_

Pawns:
    losing central pawns is bad
    - positive values tend toward the centre
    - flank pawns are less common to be pushed

    pawns protect the king
    - pawns in front of castled king should remain on start rank

    endgame pawns should be pushed
    - when there are few pieces remaining on the board
    - positive values closer to promotion rank

    passed pawns should be pushed
    - when there are no other pawns blocking promotion
    - positive values closer to promotion rank

    pawns should be sacrificed in file of opponent castled king

Knights:
    knights belong in the centre
    - positive values tend toward the centre
    - relative to number of possible moves

    knights on edge of board are always bad
    - large -ve value for knights on a,h files and 1,8 rank
    - this should encourage knights to develop quickly

Bishops:
    bishops like to have long diagonals

Rooks:
    rooks like open files
    - if there is a file with no opponent pieces -> strong +ve val for being on that file

    rooks aligned with opponent castled king 

    rooks stacking on a file/rank

    rooks attacking backward pawns

    rooks defening back-rank mate threats

Queens:
    queens like to be in the centre

    inherit from bishops & rooks?


`find_best_move(board, depth)`

## Minimax
- maximise score on turn 1 (turn % 2 = 1)
- minimise score on turn 2 (turn % 2 = 0)

## Alpha-Beta Pruning
_Skip branches that cannot affect result_

`alpha` = best already found (max side)
`beta` = worst opponent allows

## Move Ordering
1. Captures
2. Promotions
3. Checks
4. Normal (quiet)

## Transpositions
Same position can occur via many routes.
- create hashes (Zobrist)
- store position, eval, depth_searched

## Pseudo Code

### Minimax
int minimax(position, depth, maximizing)
{
    if (depth == 0)
        return evaluate(position);

    if (maximizing)
    {
        int best = -INF;
        for (move in moves)
        {
            make(move);
            best = max(best, minimax(pos, depth - 1, false));
            undo(move);
        }
        return best;
    }
    else
    {
        int best = INF;
        for (move in moves)
        {
            make(move);
            best = min(best, minimax(pos, depth - 1, true));
            undo(move);
        }
        return best;
    }
}

### Quiescence
int Quiesce( int alpha, int beta ) {
    int static_eval = Evaluate();

    // Stand Pat
    int best_value = static_eval;
    if( best_value >= beta )
        return best_value;
    if( best_value > alpha )
        alpha = best_value;

    until( every_capture_has_been_examined )  {
        MakeCapture();
        score = -Quiesce( -beta, -alpha );
        TakeBackMove();

        if( score >= beta )
            return score;
        if( score > best_value )
            best_value = score;
        if( score > alpha )
            alpha = score;
    }

    return best_value;
}

