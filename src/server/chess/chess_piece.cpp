#include "chess/chess_piece.hpp"
#include <cctype>

/* ---------- Common ---------- */

char Piece::symbol() const {
    char c;
    switch (type_) {
        case PieceType::Pawn:   c = 'P'; break;
        case PieceType::Rook:   c = 'R'; break;
        case PieceType::Knight: c = 'N'; break;
        case PieceType::Bishop: c = 'B'; break;
        case PieceType::Queen:  c = 'Q'; break;
        case PieceType::King:   c = 'K'; break;
    }
    return (color_ == Color::White) ? c : std::tolower(c);
}

/* ---------- Pawn ---------- */

bool Pawn::isValidMove(int fx, int fy, int tx, int ty) const {
    int dir = (color_ == Color::White) ? -1 : +1;

    // Forward move
    if (fx == tx && ty == fy + dir)
        return true;

    // First double move
    if (fx == tx) {
        if (color_ == Color::White && fy == 6 && ty == 4) return true;
        if (color_ == Color::Black && fy == 1 && ty == 3) return true;
    }

    // Diagonal move (capture checked at board level)
    if (std::abs(tx - fx) == 1 && ty == fy + dir)
        return true;

    return false;
}


/* ---------- Rook ---------- */

bool Rook::isValidMove(int fx, int fy, int tx, int ty) const {
    return fx == tx || fy == ty;
}

/* ---------- Knight ---------- */

bool Knight::isValidMove(int fx, int fy, int tx, int ty) const {
    int dx = std::abs(tx - fx);
    int dy = std::abs(ty - fy);
    return (dx == 2 && dy == 1) || (dx == 1 && dy == 2);
}

/* ---------- Bishop ---------- */

bool Bishop::isValidMove(int fx, int fy, int tx, int ty) const {
    return std::abs(tx - fx) == std::abs(ty - fy);
}

/* ---------- Queen ---------- */

bool Queen::isValidMove(int fx, int fy, int tx, int ty) const {
    return fx == tx || fy == ty ||
           std::abs(tx - fx) == std::abs(ty - fy);
}

/* ---------- King ---------- */

bool King::isValidMove(int fx, int fy, int tx, int ty) const {
    return std::abs(tx - fx) <= 1 && std::abs(ty - fy) <= 1;
}
