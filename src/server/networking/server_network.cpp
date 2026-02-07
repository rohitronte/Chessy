#include "server_network.hpp"

#include <algorithm>
#include <sstream>
#include <cctype>

/* ---------------- Constructor ---------------- */

ServerNetwork::ServerNetwork(boost::asio::io_context& io_context, short port)
    : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)) {

    board_.initialize();
}

/* ---------------- Start Accept ---------------- */

void ServerNetwork::start() {
    auto socket = std::make_shared<tcp::socket>(acceptor_.get_executor());

    acceptor_.async_accept(*socket,
        [this, socket](const boost::system::error_code& error) {
            handle_accept(socket, error);
        });
}

/* ---------------- Accept Client ---------------- */

void ServerNetwork::handle_accept(std::shared_ptr<tcp::socket> socket,
                                  const boost::system::error_code& error) {
    if (!error) {
        if (players_.size() < 2) {
            Color assigned =
                (players_.empty() ? Color::White : Color::Black);

            players_.push_back({ socket, assigned });

            std::string welcome =
                "Welcome! You are " +
                std::string(assigned == Color::White ? "White" : "Black") +
                "\n\n" + board_.display();

            send_to(socket, welcome);

            if (players_.size() == 2) {
                broadcast(
                    "Game started!\nWhite to move.\n\n" +
                    board_.display()
                );
            }

            auto buffer = std::make_shared<std::vector<char>>(1024);

            socket->async_read_some(boost::asio::buffer(*buffer),
                [this, socket, buffer](const boost::system::error_code& ec,
                                       std::size_t bytes) {
                    handle_read(socket, buffer, ec, bytes);
                });
        }
        else {
            send_to(socket, "Server full. Only 2 players allowed.\n");
        }
    }

    if (players_.size() < 2)
        start();   // keep accepting until 2 players connected
}

/* ---------------- Read Handler ---------------- */

void ServerNetwork::handle_read(std::shared_ptr<tcp::socket> socket,
                                std::shared_ptr<std::vector<char>> buffer,
                                const boost::system::error_code& error,
                                std::size_t bytes_transferred) {
    if (error) return;

    std::string input(buffer->begin(), buffer->begin() + bytes_transferred);

    // Normalize input to uppercase
    std::transform(input.begin(), input.end(), input.begin(),
                   [](unsigned char c){ return std::toupper(c); });

    Player* player = findPlayer(socket);
    if (!player) return;

    if (player->color != currentTurn_) {
        send_to(socket, "Not your turn!\n");
        return;
    }

    std::istringstream iss(input);
    std::string command, from, to;
    iss >> command >> from >> to;

    if (command != "MOVE" || from.size() != 2 || to.size() != 2) {
        send_to(socket, "Invalid command. Use: MOVE A2 A4\n");
        return;
    }

    auto [fx, fy] = parseAlgebraic(from);
    auto [tx, ty] = parseAlgebraic(to);

    if (fx < 0 || fy < 0 || tx < 0 || ty < 0) {
        send_to(socket, "Invalid coordinates.\n");
        return;
    }

    if (board_.movePiece(fx, fy, tx, ty)) {
        currentTurn_ =
            (currentTurn_ == Color::White ? Color::Black : Color::White);

        broadcast(
            "Move successful!\n" +
            std::string(currentTurn_ == Color::White ? "White" : "Black") +
            " to move.\n\n" +
            board_.display()
        );
    }
    else {
        send_to(socket,
            "Invalid move! Try again.\n\n" + board_.display());
    }

    // Re-arm async read
    auto newBuffer = std::make_shared<std::vector<char>>(1024);
    socket->async_read_some(boost::asio::buffer(*newBuffer),
        [this, socket, newBuffer](const boost::system::error_code& ec,
                                  std::size_t bytes) {
            handle_read(socket, newBuffer, ec, bytes);
        });
}

/* ---------------- Helpers ---------------- */

void ServerNetwork::send_to(std::shared_ptr<tcp::socket> socket,
                            const std::string& message) {
    boost::asio::async_write(*socket,
        boost::asio::buffer(message),
        [](const boost::system::error_code&, std::size_t) {});
}

void ServerNetwork::broadcast(const std::string& message) {
    for (auto& p : players_)
        send_to(p.socket, message);
}

Player* ServerNetwork::findPlayer(std::shared_ptr<tcp::socket> socket) {
    for (auto& p : players_)
        if (p.socket == socket)
            return &p;
    return nullptr;
}

std::pair<int,int> ServerNetwork::parseAlgebraic(const std::string& pos) const {
    int x = pos[0] - 'A';        // A–H → 0–7
    int y = 8 - (pos[1] - '0');  // 1–8 → 7–0

    if (x < 0 || x > 7 || y < 0 || y > 7)
        return { -1, -1 };

    return { x, y };
}
