# Chessy ♟️

A simple multiplayer chess game in C++ that runs in the terminal.

Two players connect to a server and play turn-by-turn.  
The server keeps the board state and validates moves.

This project focuses on backend logic and networking, not UI.

---

## What this project shows

- C++ OOP design
- Socket programming with Boost.Asio
- Client-server architecture
- Turn-based game logic
- Chess rule validation
- Async networking basics

---

## Current Features

- 8x8 ASCII chess board
- Two-player turn system
- Move input like: `MOVE E2 E4`
- Legal move validation
- Prevent capturing own pieces
- Path blocking for sliding pieces
- Check detection
- Basic checkmate detection
- Castling
- En passant
- Server-side game state

---

## Project Structure

```
src/
  server/
  client/
include/
tests/
```

---

## Build

Requirements:
- C++17+
- CMake
- Boost

Build:

```bash
cmake -S . -B build
cmake --build build
```

---

## Run

Start server:

```bash
./build/chess_server
```

Start client (open two terminals):

```bash
./build/chess_client
```

Example move:

```
MOVE E2 E4
```

---

## Why I built this

I wanted to practice:
- Real networking in C++
- Game state management
- Clean backend design
- Writing testable logic

---

## Next Improvements

- Pawn promotion choices
- Draw/stalemate rules
- Timers
- Matchmaking
- Simple GUI

---

Made by Rohit Ronte
