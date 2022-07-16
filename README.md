# Play the game

This is a simple chessboard for human v human. It doesn't do anything fancy
like block illegal moves. You can drag pieces wherever you wish. Take a piece
by placing it on top of an opponent's piece.

## Features

- resizing the window scales the game artwork
- last move indicated by a highlighted square
- undo/redo history contains the entire game
- display captured pieces
- pawn promotion

## Controls

- mouse - left-click drag to move pieces
- u - undo a move
- r - redo a move
- TAB - toggle debug overlay

### Controls for Pawn promotion

- q - promote to queen
- c - promote to rook
- b - promote to bishop
- k - promote to knight

# Environment

Windows, install MSYS. Install packages for `make`, `gcc`, and `ctags`.

# Build

```
make m
```

# Run

```
$ ./m.exe
```

# Dependencies

Install MSYS packages for `SDL2`, `SDL2_image`, and `SDL2_ttf`.

# More documentation

See https://github.com/theendofeverything/spritesheet.git

I use the same work flow on that project.
