#include "server_network.hpp"
#include "common.hpp"

ServerNetwork::ServerNetwork(boost::asio::io_context& io_context, short port)
    : acceptor_(io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)) {}

void ServerNetwork::start() {
    auto socket = std::make_shared<boost::asio::ip::tcp::socket>(acceptor_.get_executor());
    acceptor_.async_accept(*socket, [this, socket](const boost::system::error_code& error) {
        handle_accept(socket, error);
    });
}

void ServerNetwork::handle_accept(std::shared_ptr<boost::asio::ip::tcp::socket> socket, const boost::system::error_code& error) {
    if (!error) {
        std::cout << "Client connected!" << std::endl;
        auto buffer = std::make_shared<std::vector<char>>(1024);
        socket->async_read_some(boost::asio::buffer(*buffer), [this, socket, buffer](const boost::system::error_code& error, std::size_t bytes) {
            handle_read(socket, buffer, error, bytes);
        });
    }
    start();  // Accept next client
}

void ServerNetwork::handle_read(std::shared_ptr<boost::asio::ip::tcp::socket> socket, std::shared_ptr<std::vector<char>> buffer, const boost::system::error_code& error, std::size_t bytes_transferred) {
    if (!error) {
        std::string msg(buffer->begin(), buffer->begin() + bytes_transferred);
        std::cout << "Received: " << msg << std::endl;
        // Echo back
        boost::asio::async_write(*socket, boost::asio::buffer("Echo: " + msg), [](const boost::system::error_code&, std::size_t) {});
    }
}
