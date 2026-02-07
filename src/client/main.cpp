#include "networking/client_network.hpp"
#include <boost/asio.hpp>
#include <iostream>
#include <thread>

int main() {
    boost::asio::io_context io_context;

    ClientNetwork client(io_context, "127.0.0.1", 12345);
    client.start();   // connect to server

    // Run networking in background
    std::thread networkThread([&io_context]() {
        io_context.run();
    });

    // Read user input and send moves
    while (true) {
        std::string input;
        std::getline(std::cin, input);
        if (!input.empty()) {
            client.sendMessage(input + "\n");
        }
    }

    networkThread.join();
    return 0;
}
