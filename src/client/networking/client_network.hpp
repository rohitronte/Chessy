#ifndef CLIENT_NETWORK_HPP
#define CLIENT_NETWORK_HPP

#include <boost/asio.hpp>
#include <string>
#include <vector>
#include <iostream>

class ClientNetwork {
public:
    ClientNetwork(boost::asio::io_context& io_context,
                  const std::string& host,
                  short port);

    void connect_and_send(const std::string& message);

private:
    void handle_connect(const boost::system::error_code& error);
    void handle_write(const boost::system::error_code& error,
                      std::size_t bytes_transferred);
    void handle_read(const boost::system::error_code& error,
                     std::size_t bytes_transferred);

    boost::asio::ip::tcp::socket socket_;
    std::string host_;
    short port_;
    std::vector<char> buffer_;
};

#endif // CLIENT_NETWORK_HPP

