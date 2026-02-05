#include "client_network.hpp"
#include "common.hpp"

ClientNetwork::ClientNetwork(boost::asio::io_context& io_context, const std::string& host, short port)
    : socket_(io_context), host_(host), port_(port), buffer_(1024) {}

void ClientNetwork::connect_and_send(const std::string& message) {
    boost::asio::ip::tcp::resolver resolver(socket_.get_executor());
    auto endpoints = resolver.resolve(host_, std::to_string(port_));
    boost::asio::async_connect(socket_, endpoints, [this, message](const boost::system::error_code& error, const boost::asio::ip::tcp::endpoint&) {
        handle_connect(error);
        if (!error) {
            boost::asio::async_write(socket_, boost::asio::buffer(message), [this](const boost::system::error_code& error, std::size_t) {
                handle_write(error, 0);
            });
        }
    });
}

void ClientNetwork::handle_connect(const boost::system::error_code& error) {
    if (error) {
        std::cerr << "Connect failed: " << error.message() << std::endl;
    } else {
        std::cout << "Connected to server!" << std::endl;
    }
}

void ClientNetwork::handle_write(const boost::system::error_code& error, std::size_t) {
    if (!error) {
        socket_.async_read_some(boost::asio::buffer(buffer_), [this](const boost::system::error_code& error, std::size_t bytes) {
            handle_read(error, bytes);
        });
    }
}

void ClientNetwork::handle_read(const boost::system::error_code& error, std::size_t bytes_transferred) {
    if (!error) {
        std::string response(buffer_.begin(), buffer_.begin() + bytes_transferred);
        std::cout << "Server response: " << response << std::endl;
    }
}
