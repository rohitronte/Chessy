#include "networking/server_network.hpp"
#include <boost/asio.hpp>
#include <iostream> 

int main() {
    boost::asio::io_context io_context;
    ServerNetwork server(io_context, 12345);  // Port 12345
    server.start();
    std::cout << "Server running on port 12345..." << std::endl;
    io_context.run();  // Run event loop
    return 0;
}
