#ifndef CHESS_PIECE_HPP
#define CHESS_PIECE_HPP

#include <string>
#include <cmath>

enum class Color { White, Black };
enum class PieceType { Pawn, Rook, Knight, Bishop, Queen, King };

class Piece {

protected:
    Color color_;
    PieceType type_;

public:
    Piece(Color color, PieceType type) : color_(color), type_(type) {}
    virtual ~Piece() = default;

    Color getColor() const { return color_; }
    PieceType getType() const { return type_; }

    virtual bool isValidMove(int fx, int fy, int tx, int ty) const = 0;
    char symbol() const;
};

/* ---- Pieces ---- */

class Pawn : public Piece {
public:
    Pawn(Color c) : Piece(c, PieceType::Pawn) {}
    bool isValidMove(int fx, int fy, int tx, int ty) const override;
};

class Rook : public Piece {
public:
    Rook(Color c) : Piece(c, PieceType::Rook) {}
    bool isValidMove(int fx, int fy, int tx, int ty) const override;
};

class Knight : public Piece {
public:
    Knight(Color c) : Piece(c, PieceType::Knight) {}
    bool isValidMove(int fx, int fy, int tx, int ty) const override;
};

class Bishop : public Piece {
public:
    Bishop(Color c) : Piece(c, PieceType::Bishop) {}
    bool isValidMove(int fx, int fy, int tx, int ty) const override;
};

class Queen : public Piece {
public:
    Queen(Color c) : Piece(c, PieceType::Queen) {}
    bool isValidMove(int fx, int fy, int tx, int ty) const override;
};

class King : public Piece {
public:
    King(Color c) : Piece(c, PieceType::King) {}
    bool isValidMove(int fx, int fy, int tx, int ty) const override;
};

#endif
