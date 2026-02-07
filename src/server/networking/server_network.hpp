#ifndef SERVER_NETWORK_HPP
#define SERVER_NETWORK_HPP

#include <boost/asio.hpp>
#include <memory>
#include <vector>
#include <string>
#include <utility>

#include "chess/chess_board.hpp"

using boost::asio::ip::tcp;

/* ---------------- Player ---------------- */

struct Player {
    std::shared_ptr<tcp::socket> socket;
    Color color;
};

/* ------------- ServerNetwork ------------ */

class ServerNetwork {
public:
    ServerNetwork(boost::asio::io_context& io_context, short port);
    void start();

private:
    // Networking
    void handle_accept(std::shared_ptr<tcp::socket> socket,
                       const boost::system::error_code& error);

    void handle_read(std::shared_ptr<tcp::socket> socket,
                     std::shared_ptr<std::vector<char>> buffer,
                     const boost::system::error_code& error,
                     std::size_t bytes_transferred);

    void send_to(std::shared_ptr<tcp::socket> socket,
                 const std::string& message);

    void broadcast(const std::string& message);

    // Game helpers
    std::pair<int,int> parseAlgebraic(const std::string& pos) const;
    Player* findPlayer(std::shared_ptr<tcp::socket> socket);

private:
    tcp::acceptor acceptor_;

    ChessBoard board_;
    std::vector<Player> players_;   // max 2
    Color currentTurn_ = Color::White;
};

#endif
