find_package(Catch2 REQUIRED)

add_executable(chess_tests
    test_chess_board.cpp

    ${PROJECT_SOURCE_DIR}/src/server/chess/chess_board.cpp
    ${PROJECT_SOURCE_DIR}/src/server/chess/chess_piece.cpp
)

target_include_directories(chess_tests PRIVATE
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/src/server/chess
)

target_link_libraries(chess_tests
    Catch2::Catch2WithMain
)

add_test(NAME ChessTests COMMAND chess_tests)
