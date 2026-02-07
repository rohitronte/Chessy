#ifndef CLIENT_NETWORK_HPP
#define CLIENT_NETWORK_HPP

#include <boost/asio.hpp>
#include <memory>
#include <string>
#include <vector>

using boost::asio::ip::tcp;

class ClientNetwork {
public:
    ClientNetwork(boost::asio::io_context& io_context,
                  const std::string& host,
                  short port);

    void start();
    void sendMessage(const std::string& message);

private:
    void handle_connect(const boost::system::error_code& error);
    void handle_read(std::shared_ptr<std::vector<char>> buffer,
                     const boost::system::error_code& error,
                     std::size_t bytes_transferred);

private:
    tcp::resolver resolver_;
    tcp::socket socket_;
    std::string host_;
    short port_;
};

#endif
