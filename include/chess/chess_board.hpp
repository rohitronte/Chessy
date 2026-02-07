#ifndef CHESS_BOARD_HPP
#define CHESS_BOARD_HPP

#include "chess_piece.hpp"
#include <memory>
#include <array>
#include <string>

class ChessBoard {
private:
    // Castling state
    bool whiteKingMoved_  = false;
    bool blackKingMoved_  = false;

    bool whiteRookAMoved_ = false; // A1 rook
    bool whiteRookHMoved_ = false; // H1 rook
    bool blackRookAMoved_ = false; // A8 rook
    bool blackRookHMoved_ = false; // H8 rook

    bool isPathClear(int fromX, int fromY, int toX, int toY) const;

    struct EnPassantInfo {
        bool valid = false;
        int x = -1;
        int y = -1;
    } enPassant_;

private:
    std::array<std::array<std::unique_ptr<Piece>, 8>, 8> board_;

public:
    ChessBoard();
    void initialize();
    bool movePiece(int fx, int fy, int tx, int ty);
    bool isKingInCheck(Color kingColor) const;
    bool isCheckmate(Color color);
    const Piece* getPiece(int x, int y) const {
        return board_[y][x].get();
    }


    std::string display() const;
};

#endif
