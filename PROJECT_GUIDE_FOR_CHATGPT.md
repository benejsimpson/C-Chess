# ChessPP Project Guide for ChatGPT

This document is meant to be pasted into ChatGPT (or another coding assistant) so it can quickly understand the current state of the project and give useful help without having to rediscover the structure from scratch.

## Project Summary

This is a C++ chess project with:

- A core chess engine layer in `include/core` and `src/core`
- A GUI layer built with SFML in `include/gui` and `src/gui`
- A current runnable app entrypoint in `src/app/main.cpp`

The project is still in-progress. The codebase already supports:

- Board representation
- Piece placement and board helpers
- FEN loading and exporting
- Basic legal move generation
- Move application
- A playable SFML board UI with drag-and-drop
- Board flipping
- FEN copy/load in the GUI
- Move highlighting, check highlight, illegal move flash, and promotion popup

The project does not yet fully support a complete chess engine feature set. Some rules and systems are partial or unfinished.

## Top-Level Folder Structure

- `assets/`
  Contains image assets for the GUI, mainly piece sprites in `assets/pieces`.

- `include/core/`
  Public headers for the chess logic.

- `include/gui/`
  Public headers for the SFML GUI layer.

- `src/core/`
  Implementations for the chess logic.

- `src/gui/`
  Implementations for the GUI.

- `src/app/`
  App-specific launch/build area. Right now `src/app/main.cpp` is a single-translation-unit entrypoint that includes the current engine and GUI implementation files so the project can be built from one file.

- `output/`
  Built executables and copied runtime DLLs.

## Current Entrypoints

There are multiple `main` files in the repo, but they do not all serve the same purpose.

- `src/app/main.cpp`
  This is the current practical app entrypoint to use. It launches the SFML GUI and currently includes the implementation `.cpp` files directly so the current game can be built from this one file.

- `src/gui/main.cpp`
  A minimal GUI launcher. Conceptually correct, but not the main file currently being used for the bundled app build.

- `src/core/test.cpp`
  An older console test file for move generation experiments. This is not the main app.

## Architecture Overview

The code is split into two main layers:

1. Core chess logic
2. GUI and user interaction

The intended flow is:

1. `ChessGui` owns a `Game` object.
2. `Game` owns a `Board`.
3. The GUI asks `Game` for legal moves, board state, status text, and FEN.
4. Input events create `Move` attempts.
5. `Game::try_make_move()` validates the move against generated legal moves.
6. If valid, the move is applied to the board.
7. The renderer draws the updated board and UI.

## Core Layer

### `include/core/board.hpp`

This is the foundation of the engine layer.

It defines:

- `Piece`
  Encodes piece colour and type using bit flags.

- `BitBoard`
  A light wrapper around a `uint64_t` with helper operators for setting, clearing, and checking bits.

- `Board`
  The main board state object. It contains:
  - `squares[64]` for piece-per-square state
  - `bitboards[12]` for piece-specific occupancy tracking
  - `white_to_move`
  - castling rights
  - `en_passant_square`
  - `fullmove_number`

It also declares helpers for:

- Clearing/resetting the board
- Square index conversions
- Piece colour/type checks
- Piece placement/removal/movement
- Start position loading
- Piece-to-char / char-to-piece conversion

### `src/core/board.cpp`

Implements the basic board helpers.

Important responsibilities:

- `clear_board(Board&)`
  Wipes the board and resets metadata.

- `reset_board(Board&)`
  Clears the board and loads the starting position.

- `place_piece`, `remove_piece`, `move_piece`
  Keep `squares[]` and `bitboards[]` in sync.

- File/rank/index conversions
  These are used throughout both engine and GUI code.

- `load_start_position`
  Loads the starting FEN.

- `piece_to_char`, `char_to_piece`
  Used for display and FEN parsing/export.

### `include/core/fen.hpp` and `src/core/fen.cpp`

Handles FEN-related functionality.

Current supported behavior:

- `load_fen(Board&, const std::string&)`
  Parses the board placement section and active colour.

- `export_fen(const Board&)`
  Outputs board placement plus side to move.

Important limitation:

- Full FEN support is not implemented yet.
- Castling rights, en passant target, halfmove clock, and fullmove number are not fully round-tripped in the exported FEN.
- The GUI currently uses a simplified FEN format like:
  - `"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w"`

### `include/core/move.hpp`

Defines the move model.

Important types:

- `MoveFlag`
  Includes:
  - `QUIET`
  - `CAPTURE`
  - `DOUBLE_PAWN`
  - `EN_PASSANT`
  - `KING_CASTLE`
  - `QUEEN_CASTLE`
  - `PROMOTION`
  - `PROMO_CAPTURE`

- `Move`
  Contains:
  - `from`
  - `to`
  - `piece`
  - `captured`
  - `flag`
  - `promotion`

Important helpers:

- `create_move(...)`
- `is_capture(...)`
- `is_promotion(...)`

### `include/core/makemove.hpp` and `src/core/makemove.cpp`

This is the current move-application implementation the app uses.

It handles:

- Standard moves
- Captures
- Promotion
- Promotion captures
- En passant bookkeeping
- Castling-right updates
- Castling move handling
- Fullmove number increments

This file should be treated as the authoritative move application path for the current app.

### `src/core/move.cpp`

This file also defines `apply_move(...)`, but it is an older/simpler implementation.

Important note for ChatGPT:

- `src/core/move.cpp` is effectively legacy code now.
- It only handles basic moves and promotion.
- It does not represent the more complete move application logic in `src/core/makemove.cpp`.
- When suggesting fixes, prefer `makemove.cpp` unless the goal is explicitly to clean up or remove duplicate legacy code.

### `include/core/movegen.hpp` and `src/core/movegen.cpp`

Handles move generation and move legality.

Main functions:

- `generate_pseudo_legal_moves(const Board&)`
- `generate_legal_moves(const Board&)`
- `generate_legal_moves_for_square(const Board&, int square)`
- `same_move(...)`
- `is_in_check(...)`

Current move generation behavior:

- Pawns
  - single push
  - double push
  - captures
  - promotions
  - promotion captures

- Knights
  - standard knight moves

- Bishops
  - sliding diagonal moves

- Rooks
  - sliding orthogonal moves

- Queens
  - bishop + rook move generation

- Kings
  - one-square king moves

Legality model:

- Pseudo-legal moves are generated first.
- Each move is tested on a copied board.
- A move is legal if it does not leave the mover's king in check.

Current limitations:

- Castling generation is not implemented yet, even though move application has castling support.
- En passant generation is not implemented yet, even though move application tracks `en_passant_square`.
- So the system contains some rule infrastructure that is ahead of the current generator.

### `include/core/utils.h`

Very small shared utility header.

Currently provides:

- standard library includes
- `newl` constant

This is more of a convenience/include bucket than a full utility layer.

## GUI Layer

### `include/gui/game.hpp` and `src/gui/game.cpp`

`Game` is the bridge between the GUI and the engine.

It wraps a `Board` and exposes a simpler interface for the GUI:

- `get_board()`
- `get_legal_moves()`
- `get_legal_moves_for_square(int)`
- `try_make_move(const Move&)`
- `reset_to_start()`
- `load_position_from_fen(...)`
- `get_fen()`
- `white_to_move()`
- `is_check()`
- `get_status_text()`
- last-move tracking helpers

This class is important because the GUI does not talk directly to raw core functions most of the time. It talks to `Game`.

### `include/gui/gui.hpp` and `src/gui/gui.cpp`

`ChessGui` is the top-level GUI application object.

It owns:

- `sf::RenderWindow window`
- `sf::Font font`
- `Game game`
- `GuiState gui`
- `TextureManager textures`
- `Renderer renderer`
- `InputHandler input`
- UI widgets:
  - `flip_button`
  - `copy_fen_button`
  - `load_fen_button`
  - `fen_box`

Main methods:

- `init()`
  - creates the window
  - loads a font
  - loads piece textures
  - resets the game
  - initializes GUI state and FEN box

- `run()`
  - polls SFML events
  - forwards them to `InputHandler`
  - updates GUI state
  - syncs the FEN textbox
  - calls `Renderer::draw(...)`

### `include/gui/gui_state.hpp`

This is the transient UI state model.

It stores things that are not part of chess rules themselves, including:

- selected square
- selected legal moves
- dragging state
- hovered square during drag
- dragged piece
- board orientation
- illegal move flash state
- promotion popup state
- FEN input text and typing state

This separation is useful:

- `Board` is the actual chess position
- `GuiState` is the visual/input interaction state

### `include/gui/input.hpp` and `src/gui/input.cpp`

Handles all GUI interaction.

Responsibilities:

- window close and escape handling
- mouse motion
- mouse press/release
- dragging pieces
- square selection
- trying moves by click or drag-drop
- promotion popup choice handling
- FEN textbox typing and paste handling
- button actions:
  - flip board
  - copy FEN
  - load FEN
- illegal move flash timing

Important interaction flow:

1. User clicks a square or starts dragging a piece.
2. `InputHandler` checks that the piece belongs to the side to move.
3. It loads legal moves for that square from `Game`.
4. If the user drops/clicks onto a legal destination:
   - promotion popup may appear
   - otherwise `Game::try_make_move()` is called
5. GUI state is cleared or updated appropriately.

### `include/gui/renderer.hpp` and `src/gui/renderer.cpp`

Draws everything in the window.

Responsibilities:

- chessboard tiles
- last move highlight
- selected square highlight
- legal move dots
- check highlight on the king
- board pieces
- dragged piece sprite
- side panel
- promotion popup
- illegal move flash

It uses:

- `Game` for board and status information
- `GuiState` for UI state
- `TextureManager` for sprites
- `sf::Font` for text

### `include/gui/textures.hpp` and `src/gui/textures.cpp`

Loads and serves piece textures.

Responsibilities:

- load all piece PNGs from `assets/pieces`
- map `Piece` values to the correct texture slot using `piece_to_bitboard_index`

### `include/gui/widgets.hpp`

Defines very small widget structs used by the GUI:

- `Button`
- `TextBox`

These are lightweight data holders, not a full widget system.

### `include/gui/constants.hpp`

Stores GUI layout constants such as:

- window size
- board size
- square size
- board position
- side panel position

## Assets and Runtime Files

### `assets/pieces/`

Contains separate PNGs for each piece:

- `wp.png`, `wn.png`, `wb.png`, `wr.png`, `wq.png`, `wk.png`
- `bp.png`, `bn.png`, `bb.png`, `br.png`, `bq.png`, `bk.png`

### `src/app/*.dll` and `output/*.dll`

These are runtime DLLs needed for the Windows executable, especially SFML and its dependencies.

## Current Runtime Flow

When the app starts:

1. `main()` creates `ChessGui`
2. `ChessGui::init()` creates the window, loads fonts and textures, and resets the game
3. `ChessGui::run()` enters the event/render loop
4. User input is processed by `InputHandler`
5. Legal moves come from `Game`, which uses the core move generator
6. If a move is accepted, `Game` applies it to the `Board`
7. `Renderer` draws the latest board/UI state every frame

## Important Current Limitations and Quirks

These are important to tell ChatGPT so it does not assume the project is more complete than it is.

### 1. Single-file build workaround in `src/app/main.cpp`

`src/app/main.cpp` currently includes `.cpp` implementation files directly.

That is not a normal long-term architecture choice. It is a practical build workaround so the current project state can be compiled from one file.

If the project is cleaned up later, the likely direction is:

- compile `src/core/*.cpp` separately
- compile `src/gui/*.cpp` separately
- keep `src/app/main.cpp` as only a small launcher

### 2. Duplicate move application implementations

There are two different `apply_move(...)` implementations:

- `src/core/move.cpp`
- `src/core/makemove.cpp`

For the current app, `makemove.cpp` is the one to treat as correct/current.

### 3. Move generation and move application are not fully aligned yet

Examples:

- Castling application exists, but castling move generation does not.
- En passant bookkeeping exists, but en passant move generation does not.

This means some rule infrastructure is partial.

### 4. FEN support is simplified

The code mainly handles:

- board placement
- side to move

It does not yet fully preserve all standard FEN fields.

### 5. `Game::load_position_from_fen(...)` currently always returns `true`

Right now it calls `load_fen(...)`, clears last-move state, and returns `true` without validating malformed input.

This matters because the GUI assumes FEN loading can fail, but the current implementation does not really report failure.

### 6. `src/core/test.cpp` is old experimental code

It should not be treated as the main engine API design.

## Suggested Mental Model for Working on This Project

If ChatGPT is helping with a feature, it should usually think in this order:

1. Does the feature belong to chess rules?
   - If yes, inspect `Board`, `Move`, `movegen.cpp`, `makemove.cpp`, and `fen.cpp`.

2. Does the feature belong to game state as exposed to the UI?
   - If yes, inspect `Game`.

3. Does the feature belong to visual state or user interaction?
   - If yes, inspect `GuiState`, `InputHandler`, `Renderer`, `textures`, and widget definitions.

## Good File Starting Points by Task

If the task is about:

- board representation
  - `include/core/board.hpp`
  - `src/core/board.cpp`

- FEN import/export
  - `include/core/fen.hpp`
  - `src/core/fen.cpp`

- move representation
  - `include/core/move.hpp`

- legal move generation
  - `include/core/movegen.hpp`
  - `src/core/movegen.cpp`

- applying moves and special rules
  - `include/core/makemove.hpp`
  - `src/core/makemove.cpp`

- GUI app structure
  - `include/gui/gui.hpp`
  - `src/gui/gui.cpp`

- click/drag interaction
  - `include/gui/input.hpp`
  - `src/gui/input.cpp`

- rendering
  - `include/gui/renderer.hpp`
  - `src/gui/renderer.cpp`

- the wrapper between GUI and engine
  - `include/gui/game.hpp`
  - `src/gui/game.cpp`

## What ChatGPT Should Avoid Assuming

- Do not assume full legal chess rules are implemented.
- Do not assume complete FEN support.
- Do not assume the codebase is already normalized around one `apply_move(...)` implementation.
- Do not assume `src/app/main.cpp` is the final intended architecture.
- Do not assume the GUI has a generalized widget framework; it uses simple structs and custom handling.

## Short Summary for ChatGPT

This is an in-progress C++ chess project with a separate core engine layer and an SFML GUI layer. `Game` is the bridge between GUI and engine. `Board` is the core state object. Legal moves come from `movegen.cpp`, while move execution should be treated as coming from `makemove.cpp`. The current app runs through `src/app/main.cpp`, which temporarily includes multiple `.cpp` files directly to make the project easy to compile as a single entrypoint. The GUI already supports piece movement, drag-and-drop, highlighting, promotion UI, and simplified FEN load/copy, but some chess rules and validation paths are still incomplete.
