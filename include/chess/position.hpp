#ifndef POSITION_HPP
#define POSITION_HPP

#include <string>
#include <cctype>

/*
 * Position represents a square on the chess board.
 * Internally:
 *   row = 0 -> rank 8
 *   row = 7 -> rank 1
 *   col = 0 -> file A
 *   col = 7 -> file H
 */
class Position {
public:
    int row; // 0–7
    int col; // 0–7

    Position() : row(-1), col(-1) {}
    Position(int r, int c) : row(r), col(c) {}

    // Check if position is inside the board
    bool isValid() const {
        return row >= 0 && row < 8 && col >= 0 && col < 8;
    }

    // Convert algebraic notation (e.g., "A2") to Position
    static Position fromAlgebraic(const std::string& pos) {
        if (pos.size() != 2)
            return Position();

        char file = std::toupper(pos[0]); // A-H
        char rank = pos[1];               // 1-8

        if (file < 'A' || file > 'H' || rank < '1' || rank > '8')
            return Position();

        int col = file - 'A';
        int row = 8 - (rank - '0');

        return Position(row, col);
    }

    // Equality comparison
    bool operator==(const Position& other) const {
        return row == other.row && col == other.col;
    }

    bool operator!=(const Position& other) const {
        return !(*this == other);
    }
};

#endif
