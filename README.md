# C++ Chess

My second C++ project - Using bitboards to make a chess game and, eventually, an engine!

## Project Summary

This is a C++ chess project with:

- A core chess engine layer in `include/core` and `src/core`
- A GUI layer built with SFML in `include/gui` and `src/gui`
- A current runnable app entrypoint in `src/app/main.cpp`

This project is still in-progress. The codebase already supports:

- Board representation
- Piece placement and board helpers
- FEN loading and exporting
- Basic legal move generation
- Move application
- A playable SFML board UI with drag-and-drop
- Board flipping
- FEN copy/load in the GUI
- Move highlighting, check highlight, illegal move flash, and promotion popup

This does not yet fully support a complete chess engine feature set. Some rules and systems are unfinished.

## Architecture Overview

Code split into two main layers:

1. Core chess logic
2. GUI and user interaction

How it works:

1. ChessGui owns a Game object.
2. Game owns a Board.
3. The GUI asks Game for legal moves, board state, status text, and FEN.
4. Input events create Move attempts.
5. Game::try_make_move() validates the move against generated legal moves.
6. If valid, the move is applied to the board.
7. The renderer draws the updated board and UI.
