#include "client_network.hpp"

#include <iostream>

/* ---------------- Constructor ---------------- */

ClientNetwork::ClientNetwork(boost::asio::io_context& io_context,
                             const std::string& host,
                             short port)
    : resolver_(io_context),
      socket_(io_context),
      host_(host),
      port_(port) {}

/* ---------------- Start Client ---------------- */

void ClientNetwork::start() {
    auto endpoints = resolver_.resolve(host_, std::to_string(port_));

    boost::asio::async_connect(
        socket_,
        endpoints,
        [this](const boost::system::error_code& error, const tcp::endpoint&) {
            handle_connect(error);
        }
    );
}

/* ---------------- Connect Handler ---------------- */

void ClientNetwork::handle_connect(const boost::system::error_code& error) {
    if (!error) {
        std::cout << "Connected to server.\n\n";

        auto buffer = std::make_shared<std::vector<char>>(2048);

        socket_.async_read_some(
            boost::asio::buffer(*buffer),
            [this, buffer](const boost::system::error_code& ec,
                           std::size_t bytes) {
                handle_read(buffer, ec, bytes);
            }
        );
    }
    else {
        std::cerr << "Connection failed: "
                  << error.message() << std::endl;
    }
}

/* ---------------- Read Handler ---------------- */

void ClientNetwork::handle_read(std::shared_ptr<std::vector<char>> buffer,
                                const boost::system::error_code& error,
                                std::size_t bytes_transferred) {
    if (error) {
        std::cerr << "Disconnected from server.\n";
        return;
    }

    std::string message(buffer->begin(),
                        buffer->begin() + bytes_transferred);

    std::cout << message << std::endl;

    // Re-arm read for continuous updates
    auto newBuffer = std::make_shared<std::vector<char>>(2048);

    socket_.async_read_some(
        boost::asio::buffer(*newBuffer),
        [this, newBuffer](const boost::system::error_code& ec,
                          std::size_t bytes) {
            handle_read(newBuffer, ec, bytes);
        }
    );
}

/* ---------------- Send Message ---------------- */

void ClientNetwork::sendMessage(const std::string& message) {
    boost::asio::async_write(
        socket_,
        boost::asio::buffer(message),
        [](const boost::system::error_code&, std::size_t) {}
    );
}
