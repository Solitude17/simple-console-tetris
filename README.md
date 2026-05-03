# Tetris — Console Tetris in C

A classic Tetris game implemented in C for the terminal, built with ncurses.

## Features

- ✅ Classic Tetris gameplay with 7 standard tetrominoes
- ✅ Real-time keyboard controls (arrows, rotation, hard drop)
- ✅ Score and level tracking
- ✅ Terminal-based interface using ncurses
- ✅ Unit tests with check framework

## Dependencies

- **gcc** — C compiler
- **make** — build system
- **ncurses** — terminal UI rendering
- **check** — unit testing framework

### Install on Linux (Ubuntu/Debian)

sudo apt update
sudo apt install gcc make libncurses-dev check

### Install on Windows (MSYS2)

1. Download and install MSYS2
2. Open **MSYS2 UCRT64** terminal and run:

pacman -Syu
pacman -S mingw-w64-ucrt-x86_64-gcc make mingw-w64-ucrt-x86_64-ncurses mingw-w64-ucrt-x86_64-check


## Build & Run

### Build the game

make

### Run the game

make run

### Run tests

make test

## Supported Platforms

| Platform | Status | Notes |
| :--- | :--- | :--- |
| Linux | ✅ Full support | Native GCC + ncurses |
| Windows | ⚠️ Via MSYS2 | See installation above |
| macOS | ❓ Not tested | Should work with `brew install ncurses check` |
