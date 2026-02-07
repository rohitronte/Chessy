cmake_minimum_required(VERSION 3.16)
project(ChessApp VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Homebrew include path

include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(/opt/homebrew/include)

# Find Boost (header-only)
find_package(Boost REQUIRED)

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
endif()

# Server executable
add_executable(chess_server
    src/server/main.cpp
    src/server/networking/server_network.cpp

    # Chess engine (IMPORTANT)
    src/server/chess/chess_board.cpp
    src/server/chess/chess_piece.cpp
)

# Client executable
add_executable(chess_client
    src/client/main.cpp
    src/client/networking/client_network.cpp
)

# No Boost::system linking needed!

enable_testing()
add_subdirectory(tests)


