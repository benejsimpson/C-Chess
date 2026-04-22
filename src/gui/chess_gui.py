
import tkinter as tk
from tkinter import ttk, filedialog, messagebox
import subprocess

START_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"

UNICODE_PIECES = {
    "P": "♙", "N": "♘", "B": "♗", "R": "♖", "Q": "♕", "K": "♔",
    "p": "♟", "n": "♞", "b": "♝", "r": "♜", "q": "♛", "k": "♚",
}

FILES = "abcdefgh"


class ChessGUI:
    def __init__(self, root: tk.Tk) -> None:
        self.root = root
        self.root.title("Chess GUI")
        self.square_size = 80
        self.selected = None
        self.side_to_move = "w"
        self.castling = "KQkq"
        self.en_passant = "-"
        self.halfmove = 0
        self.fullmove = 1
        self.board = [["" for _ in range(8)] for _ in range(8)]

        self.engine_path_var = tk.StringVar()
        self.engine_reply_var = tk.StringVar(value="Engine output will appear here.")
        self.fen_var = tk.StringVar(value=START_FEN)

        self._build_ui()
        self.load_fen(START_FEN)
        self.draw_board()

    def _build_ui(self) -> None:
        outer = ttk.Frame(self.root, padding=10)
        outer.pack(fill="both", expand=True)

        left = ttk.Frame(outer)
        left.pack(side="left", fill="both", expand=False)

        right = ttk.Frame(outer)
        right.pack(side="left", fill="both", expand=True, padx=(12, 0))

        self.canvas = tk.Canvas(
            left,
            width=self.square_size * 8,
            height=self.square_size * 8,
            highlightthickness=1,
            highlightbackground="#444"
        )
        self.canvas.pack()
        self.canvas.bind("<Button-1>", self.on_click)

        ttk.Label(right, text="FEN").pack(anchor="w")
        self.fen_entry = ttk.Entry(right, textvariable=self.fen_var, width=90)
        self.fen_entry.pack(fill="x", pady=(0, 8))

        row1 = ttk.Frame(right)
        row1.pack(fill="x", pady=(0, 8))
        ttk.Button(row1, text="Load FEN", command=self.load_fen_from_entry).pack(side="left")
        ttk.Button(row1, text="Copy Current FEN", command=self.copy_current_fen).pack(side="left", padx=6)
        ttk.Button(row1, text="Reset Start Position", command=lambda: self.load_fen(START_FEN)).pack(side="left")

        ttk.Separator(right).pack(fill="x", pady=8)

        ttk.Label(right, text="C++ engine / executable path").pack(anchor="w")
        path_row = ttk.Frame(right)
        path_row.pack(fill="x", pady=(0, 8))
        ttk.Entry(path_row, textvariable=self.engine_path_var, width=70).pack(side="left", fill="x", expand=True)
        ttk.Button(path_row, text="Browse", command=self.browse_engine).pack(side="left", padx=(6, 0))

        ttk.Label(
            right,
            text="This GUI sends the current position to your C++ code as a FEN string via stdin."
        ).pack(anchor="w", pady=(0, 6))

        engine_row = ttk.Frame(right)
        engine_row.pack(fill="x", pady=(0, 8))
        ttk.Button(engine_row, text="Send FEN to C++", command=self.send_fen_to_engine).pack(side="left")
        ttk.Button(engine_row, text="Flip Board", command=self.flip_board).pack(side="left", padx=6)

        ttk.Label(right, text="Engine response").pack(anchor="w")
        ttk.Label(
            right,
            textvariable=self.engine_reply_var,
            wraplength=500,
            relief="solid",
            padding=8
        ).pack(fill="x", pady=(0, 8))

        instructions = (
            "How to use:\n"
            "• Click a piece, then click a destination square.\n"
            "• This GUI allows piece movement but does not enforce chess legality.\n"
            "• Current board state is always exportable as FEN.\n"
            "• Your C++ program can read that FEN and respond with text.\n\n"
            "Expected stdin for your C++ code:\n"
            "position fen <current_fen>\n\n"
            "Example C++ reply:\n"
            "bestmove e2e4"
        )
        ttk.Label(right, text=instructions, justify="left").pack(anchor="w")

    def browse_engine(self) -> None:
        path = filedialog.askopenfilename(title="Select C++ executable")
        if path:
            self.engine_path_var.set(path)

    def algebraic_to_index(self, square: str) -> tuple[int, int]:
        file_char, rank_char = square[0], square[1]
        col = FILES.index(file_char)
        row = 8 - int(rank_char)
        return row, col

    def index_to_algebraic(self, row: int, col: int) -> str:
        return f"{FILES[col]}{8 - row}"

    def load_fen_from_entry(self) -> None:
        try:
            self.load_fen(self.fen_var.get().strip())
        except Exception as exc:
            messagebox.showerror("Invalid FEN", str(exc))

    def load_fen(self, fen: str) -> None:
        parts = fen.strip().split()
        if len(parts) < 4:
            raise ValueError("FEN must have at least 4 fields.")

        placement = parts[0]
        self.side_to_move = parts[1]
        self.castling = parts[2]
        self.en_passant = parts[3]
        self.halfmove = int(parts[4]) if len(parts) > 4 else 0
        self.fullmove = int(parts[5]) if len(parts) > 5 else 1

        rows = placement.split("/")
        if len(rows) != 8:
            raise ValueError("Piece placement must contain 8 ranks.")

        board = []
        for rank in rows:
            row = []
            for char in rank:
                if char.isdigit():
                    row.extend([""] * int(char))
                else:
                    if char not in UNICODE_PIECES:
                        raise ValueError(f"Invalid piece character: {char}")
                    row.append(char)
            if len(row) != 8:
                raise ValueError("Each rank must expand to exactly 8 squares.")
            board.append(row)

        self.board = board
        self.selected = None
        self.fen_var.set(self.current_fen())
        self.draw_board()

    def current_fen(self) -> str:
        fen_rows = []
        for row in self.board:
            empty = 0
            out = []
            for piece in row:
                if piece == "":
                    empty += 1
                else:
                    if empty:
                        out.append(str(empty))
                        empty = 0
                    out.append(piece)
            if empty:
                out.append(str(empty))
            fen_rows.append("".join(out))
        placement = "/".join(fen_rows)
        return f"{placement} {self.side_to_move} {self.castling} {self.en_passant} {self.halfmove} {self.fullmove}"

    def copy_current_fen(self) -> None:
        fen = self.current_fen()
        self.root.clipboard_clear()
        self.root.clipboard_append(fen)
        self.fen_var.set(fen)
        self.engine_reply_var.set("Copied current FEN to clipboard.")

    def flip_board(self) -> None:
        self.board = [list(reversed(row)) for row in reversed(self.board)]
        self.selected = None
        self.fen_var.set(self.current_fen())
        self.draw_board()

    def draw_board(self) -> None:
        self.canvas.delete("all")
        light = "#f0d9b5"
        dark = "#b58863"
        highlight = "#f6f669"
        select = "#89cff0"

        for row in range(8):
            for col in range(8):
                x1 = col * self.square_size
                y1 = row * self.square_size
                x2 = x1 + self.square_size
                y2 = y1 + self.square_size

                color = light if (row + col) % 2 == 0 else dark
                if self.selected == (row, col):
                    color = select

                self.canvas.create_rectangle(x1, y1, x2, y2, fill=color, outline=color)

                piece = self.board[row][col]
                if piece:
                    self.canvas.create_text(
                        x1 + self.square_size / 2,
                        y1 + self.square_size / 2,
                        text=UNICODE_PIECES[piece],
                        font=("Segoe UI Symbol", 36)
                    )

        if self.selected is not None:
            row, col = self.selected
            x1 = col * self.square_size
            y1 = row * self.square_size
            x2 = x1 + self.square_size
            y2 = y1 + self.square_size
            self.canvas.create_rectangle(x1, y1, x2, y2, outline=highlight, width=4)

    def on_click(self, event) -> None:
        col = event.x // self.square_size
        row = event.y // self.square_size

        if not (0 <= row < 8 and 0 <= col < 8):
            return

        if self.selected is None:
            if self.board[row][col] != "":
                self.selected = (row, col)
        else:
            src_row, src_col = self.selected
            if (src_row, src_col) == (row, col):
                self.selected = None
            else:
                self.move_piece(src_row, src_col, row, col)
                self.selected = None

        self.fen_var.set(self.current_fen())
        self.draw_board()

    def move_piece(self, src_row: int, src_col: int, dst_row: int, dst_col: int) -> None:
        piece = self.board[src_row][src_col]
        if piece == "":
            return

        moved_piece = piece

        if piece == "P" and dst_row == 0:
            moved_piece = "Q"
        elif piece == "p" and dst_row == 7:
            moved_piece = "q"

        self.board[dst_row][dst_col] = moved_piece
        self.board[src_row][src_col] = ""

        self.side_to_move = "b" if self.side_to_move == "w" else "w"

        if self.side_to_move == "w":
            self.fullmove += 1

        self.en_passant = "-"
        self.halfmove = 0 if piece.lower() == "p" else self.halfmove + 1

    def send_fen_to_engine(self) -> None:
        engine = self.engine_path_var.get().strip()
        if not engine:
            messagebox.showwarning("No executable selected", "Select your C++ executable first.")
            return

        fen = self.current_fen()
        payload = f"position fen {fen}\n"

        try:
            completed = subprocess.run(
                [engine],
                input=payload,
                text=True,
                capture_output=True,
                timeout=5
            )
        except FileNotFoundError:
            messagebox.showerror("Executable not found", "That file could not be opened.")
            return
        except subprocess.TimeoutExpired:
            self.engine_reply_var.set("C++ process timed out after 5 seconds.")
            return
        except Exception as exc:
            self.engine_reply_var.set(f"Failed to run C++ process: {exc}")
            return

        stdout = completed.stdout.strip()
        stderr = completed.stderr.strip()

        if stdout:
            self.engine_reply_var.set(stdout)
        elif stderr:
            self.engine_reply_var.set(f"stderr: {stderr}")
        else:
            self.engine_reply_var.set("C++ program ran, but returned no output.")


def main() -> None:
    root = tk.Tk()
    style = ttk.Style()
    try:
        style.theme_use("clam")
    except tk.TclError:
        pass
    ChessGUI(root)
    root.mainloop()


if __name__ == "__main__":
    main()