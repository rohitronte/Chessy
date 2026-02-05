#ifndef SERVER_NETWORK_HPP
#define SERVER_NETWORK_HPP

#include <boost/asio.hpp>
#include <memory>
#include <thread>

class ServerNetwork {
public:
    ServerNetwork(boost::asio::io_context& io_context, short port);
    void start();

private:
    void handle_accept(std::shared_ptr<boost::asio::ip::tcp::socket> socket,
                       const boost::system::error_code& error);

    void handle_read(std::shared_ptr<boost::asio::ip::tcp::socket> socket,
                     std::shared_ptr<std::vector<char>> buffer,
                     const boost::system::error_code& error,
                     std::size_t bytes_transferred);

    boost::asio::ip::tcp::acceptor acceptor_;
};

#endif // SERVER_NETWORK_HPP
;
