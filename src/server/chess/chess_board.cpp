#include "chess/chess_board.hpp"
#include <sstream>

ChessBoard::ChessBoard() {
    for (auto& row : board_)
        for (auto& cell : row)
            cell = nullptr;
}

void ChessBoard::initialize() {
    // Black
    board_[0][0] = std::make_unique<Rook>(Color::Black);
    board_[0][1] = std::make_unique<Knight>(Color::Black);
    board_[0][2] = std::make_unique<Bishop>(Color::Black);
    board_[0][3] = std::make_unique<Queen>(Color::Black);
    board_[0][4] = std::make_unique<King>(Color::Black);
    board_[0][5] = std::make_unique<Bishop>(Color::Black);
    board_[0][6] = std::make_unique<Knight>(Color::Black);
    board_[0][7] = std::make_unique<Rook>(Color::Black);
    for (int i = 0; i < 8; ++i)
        board_[1][i] = std::make_unique<Pawn>(Color::Black);

    // White
    for (int i = 0; i < 8; ++i)
        board_[6][i] = std::make_unique<Pawn>(Color::White);
    board_[7][0] = std::make_unique<Rook>(Color::White);
    board_[7][1] = std::make_unique<Knight>(Color::White);
    board_[7][2] = std::make_unique<Bishop>(Color::White);
    board_[7][3] = std::make_unique<Queen>(Color::White);
    board_[7][4] = std::make_unique<King>(Color::White);
    board_[7][5] = std::make_unique<Bishop>(Color::White);
    board_[7][6] = std::make_unique<Knight>(Color::White);
    board_[7][7] = std::make_unique<Rook>(Color::White);
}

bool ChessBoard::movePiece(int fx, int fy, int tx, int ty) {
    // Source must have a piece
    if (!board_[fy][fx])
        return false;

    Piece* piece = board_[fy][fx].get();

    // Save en-passant state (for rollback)
    bool oldEPValid = enPassant_.valid;
    int oldEPx = enPassant_.x;
    int oldEPy = enPassant_.y;

    // We'll use these for transactional en-passant handling
    bool willDoEnPassantCapture = false;
    int epCaptureX = -1, epCaptureY = -1;
    std::unique_ptr<Piece> epCapturedPawn = nullptr; // holds the captured pawn while committing

    // Shape validation
    if (!piece->isValidMove(fx, fy, tx, ty))
        return false;

    /* ===================== CASTLING ===================== */
    if (piece->getType() == PieceType::King &&
        std::abs(tx - fx) == 2 &&
        fy == ty) {

        bool isWhite = (piece->getColor() == Color::White);

        // 1. King cannot be in check
        if (isKingInCheck(piece->getColor()))
            return false;

        // 2. King must not have moved
        if ((isWhite && whiteKingMoved_) ||
            (!isWhite && blackKingMoved_))
            return false;

        bool kingSide = (tx > fx);
        int stepX = kingSide ? fx + 1 : fx - 1;
        int rookFromX = kingSide ? 7 : 0;
        int rookToX   = kingSide ? tx - 1 : tx + 1;

        // 3. Rook existence & color
        if (!board_[fy][rookFromX] ||
            board_[fy][rookFromX]->getType() != PieceType::Rook ||
            board_[fy][rookFromX]->getColor() != piece->getColor())
            return false;

        // 4. Rook must not have moved
        if (isWhite) {
            if ((kingSide && whiteRookHMoved_) ||
                (!kingSide && whiteRookAMoved_))
                return false;
        } else {
            if ((kingSide && blackRookHMoved_) ||
                (!kingSide && blackRookAMoved_))
                return false;
        }

        // 5. Path must be clear: check squares between king and destination (exclude destination)
        int dir = kingSide ? 1 : -1;
        for (int x = fx + dir; x != tx; x += dir) {
            if (board_[fy][x])
                return false;
        }

        // 6. King cannot pass through check (simulate stepping to the intermediate square)
        {
            // Save and simulate king stepping to stepX
            auto savedKing = std::move(board_[fy][fx]);
            board_[fy][stepX] = std::move(savedKing);
            // ensure original is null (already done by move)
            bool inCheck = isKingInCheck(piece->getColor());
            // revert
            board_[fy][fx] = std::move(board_[fy][stepX]);
            board_[fy][stepX] = nullptr;

            if (inCheck)
                return false;
        }

        // ---- Perform castling ----
        board_[ty][tx] = std::move(board_[fy][fx]);
        board_[fy][fx] = nullptr;

        board_[fy][rookToX] = std::move(board_[fy][rookFromX]);
        board_[fy][rookFromX] = nullptr;

        // 7. King cannot end in check -> if so revert both king and rook
        if (isKingInCheck(piece->getColor())) {
            // revert king
            board_[fy][fx] = std::move(board_[ty][tx]);
            board_[ty][tx] = nullptr;

            // revert rook
            board_[fy][rookFromX] = std::move(board_[fy][rookToX]);
            board_[fy][rookToX] = nullptr;

            // restore en-passant (unchanged here, but keep consistent)
            enPassant_.valid = oldEPValid;
            enPassant_.x = oldEPx;
            enPassant_.y = oldEPy;

            return false;
        }

        // Update flags
        if (isWhite) {
            whiteKingMoved_ = true;
            kingSide ? whiteRookHMoved_ = true : whiteRookAMoved_ = true;
        } else {
            blackKingMoved_ = true;
            kingSide ? blackRookHMoved_ = true : blackRookAMoved_ = true;
        }

        // castling clears en-passant
        enPassant_.valid = false;
        return true;
    }

    /* ===================== EN PASSANT (detection only, no mutation) ===================== */
    // We'll *only* mark intent to perform en-passant here; actual pawn removal happens at commit time
    if (piece->getType() == PieceType::Pawn) {
        int dx = tx - fx;
        int dir = (piece->getColor() == Color::White) ? -1 : +1;

        if (dx != 0) {
            // capture move: normal capture or en-passant candidate
            if (!(board_[ty][tx] && board_[ty][tx]->getColor() != piece->getColor())) {
                // not a normal capture, maybe en-passant
                if (enPassant_.valid && tx == enPassant_.x && ty == enPassant_.y) {
                    // mark en-passant capture intent (capture pawn at (fy, tx))
                    willDoEnPassantCapture = true;
                    epCaptureX = tx;
                    epCaptureY = fy; // captured pawn sits on same rank as the capturer before move
                } else {
                    return false; // illegal diagonal move
                }
            }
        } else {
            // forward move must be empty
            if (board_[ty][tx])
                return false;
        }
    }

    /* ===================== OWN PIECE CAPTURE ===================== */
    if (board_[ty][tx] &&
        board_[ty][tx]->getColor() == piece->getColor())
        return false;

    /* ===================== PATH BLOCKING ===================== */
    PieceType type = piece->getType();
    if ((type == PieceType::Rook ||
         type == PieceType::Bishop ||
         type == PieceType::Queen) &&
        !isPathClear(fx, fy, tx, ty))
        return false;

    /* ===================== COMMIT MOVE (perform captures transactionally) ===================== */

    // If en-passant capture is intended, stash the pawn now (do not mutate board until stash)
    if (willDoEnPassantCapture) {
        // move the captured pawn into epCapturedPawn (so we can restore on rollback)
        epCapturedPawn = std::move(board_[epCaptureY][epCaptureX]);
        // board_[epCaptureY][epCaptureX] is now nullptr (captured pawn removed from board)
    }

    // Normal capture (destination may be empty)
    auto captured = std::move(board_[ty][tx]);

    // Move the moving piece
    board_[ty][tx] = std::move(board_[fy][fx]);
    board_[fy][fx] = nullptr;

    // After move commit, if en-passant was performed, the captured pawn has already been removed into epCapturedPawn
    // (we already moved it above), so nothing further to do here.

    /* ===================== POST-MOVE VALIDATION (king safety) ===================== */

    // Illegal if own king is in check -> revert everything (including en-passant capture)
    if (isKingInCheck(piece->getColor())) {
        // revert moving piece
        board_[fy][fx] = std::move(board_[ty][tx]);
        // revert destination
        board_[ty][tx] = std::move(captured);

        // restore ep pawn if it was captured
        if (epCapturedPawn) {
            board_[epCaptureY][epCaptureX] = std::move(epCapturedPawn);
        }

        // restore en-passant state
        enPassant_.valid = oldEPValid;
        enPassant_.x = oldEPx;
        enPassant_.y = oldEPy;

        return false;
    }

    /* ===================== UPDATE FLAGS (move succeeded) ===================== */

    if (piece->getType() == PieceType::King) {
        if (piece->getColor() == Color::White) whiteKingMoved_ = true;
        else blackKingMoved_ = true;
    }

    if (piece->getType() == PieceType::Rook) {
        if (piece->getColor() == Color::White) {
            if (fx == 0 && fy == 7) whiteRookAMoved_ = true;
            if (fx == 7 && fy == 7) whiteRookHMoved_ = true;
        } else {
            if (fx == 0 && fy == 0) blackRookAMoved_ = true;
            if (fx == 7 && fy == 0) blackRookHMoved_ = true;
        }
    }

    // Enable en-passant ONLY after a successful pawn double-step
    if (piece->getType() == PieceType::Pawn) {
        int dir = (piece->getColor() == Color::White) ? -1 : +1;
        if (fx == tx && ty == fy + 2 * dir) {
            enPassant_.valid = true;
            enPassant_.x = fx;
            enPassant_.y = fy + dir;
        } else {
            enPassant_.valid = false;
        }
    } else {
        // Non-pawn moves clear en-passant
        enPassant_.valid = false;
    }

    return true;
}


bool ChessBoard::isCheckmate(Color color) {
    // If king is not in check → not checkmate
    if (!isKingInCheck(color))
        return false;

    // Try ALL possible moves for this color
    for (int fy = 0; fy < 8; ++fy) {
        for (int fx = 0; fx < 8; ++fx) {
            if (!board_[fy][fx])
                continue;

            Piece* piece = board_[fy][fx].get();
            if (piece->getColor() != color)
                continue;

            // Try every destination square
            for (int ty = 0; ty < 8; ++ty) {
                for (int tx = 0; tx < 8; ++tx) {

                    // Skip same square
                    if (fx == tx && fy == ty)
                        continue;

                    // Shape validation
                    if (!piece->isValidMove(fx, fy, tx, ty))
                        continue;

                    // Prevent capturing own piece
                    if (board_[ty][tx] &&
                        board_[ty][tx]->getColor() == color)
                        continue;

                    // Path blocking
                    PieceType t = piece->getType();
                    if ((t == PieceType::Rook ||
                         t == PieceType::Bishop ||
                         t == PieceType::Queen) &&
                        !isPathClear(fx, fy, tx, ty))
                        continue;

                    // --- simulate move ---
                    auto captured = std::move(board_[ty][tx]);
                    board_[ty][tx] = std::move(board_[fy][fx]);
                    board_[fy][fx] = nullptr;

                    bool stillInCheck = isKingInCheck(color);

                    // revert
                    board_[fy][fx] = std::move(board_[ty][tx]);
                    board_[ty][tx] = std::move(captured);

                    // If ANY move escapes check → not checkmate
                    if (!stillInCheck)
                        return false;
                }
            }
        }
    }

    // No legal escape found
    return true;
}


bool ChessBoard::isKingInCheck(Color kingColor) const {
    int kingX = -1, kingY = -1;

    // Locate the king
    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            if (board_[y][x] &&
                board_[y][x]->getType() == PieceType::King &&
                board_[y][x]->getColor() == kingColor) {
                kingX = x;
                kingY = y;
                break;
            }
        }
    }

    if (kingX == -1)
        return false; // should never happen in valid game

    // Check all enemy pieces
    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            if (!board_[y][x])
                continue;

            Piece* enemy = board_[y][x].get();
            if (enemy->getColor() == kingColor)
                continue;

            // Can this piece attack the king?
            if (!enemy->isValidMove(x, y, kingX, kingY))
                continue;

            // Sliding pieces must have clear path
            PieceType t = enemy->getType();
            if (t == PieceType::Rook ||
                t == PieceType::Bishop ||
                t == PieceType::Queen) {
                if (!isPathClear(x, y, kingX, kingY))
                    continue;
            }

            // Pawn special case: only diagonal attack
            if (t == PieceType::Pawn) {
                int dir = (enemy->getColor() == Color::White) ? -1 : +1;
                if (kingY != y + dir || std::abs(kingX - x) != 1)
                    continue;
            }

            return true; // king is in check
        }
    }

    return false;
}

bool ChessBoard::isPathClear(int fromX, int fromY, int toX, int toY) const {
    int dx = (toX > fromX) - (toX < fromX);
    int dy = (toY > fromY) - (toY < fromY);

    int x = fromX + dx;
    int y = fromY + dy;

    while (x != toX || y != toY) {
        if (board_[y][x] != nullptr)
            return false;

        x += dx;
        y += dy;
    }
    return true;
}


std::string ChessBoard::display() const {
    std::ostringstream out;

    for (int y = 0; y < 8; ++y) {
        out << (8 - y) << "  ";
        for (int x = 0; x < 8; ++x) {
            out << (board_[y][x] ? board_[y][x]->symbol() : '_');
            if (x < 7) out << " ";
        }
        out << "\n";
    }
    out << "   A B C D E F G H\n";
    return out.str();
}
