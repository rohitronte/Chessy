#include "networking/client_network.hpp"
#include <boost/asio.hpp>
#include <iostream>

int main() {
    boost::asio::io_context io_context;
    ClientNetwork client(io_context, "127.0.0.1", 12345);  // Localhost
    std::string message;
    std::cout << "Enter message to send: ";
    std::getline(std::cin, message);
    client.connect_and_send(message);
    io_context.run();
    return 0;
}
