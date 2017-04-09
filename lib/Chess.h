#ifndef CHESS_H
#define CHESS_H
#include <string>
#include <vector>
#include "Moves.h"


class Chess {
public:

    // ------- Variables --------
    Board board;
    GameType game_type;


    // ------- Constructors --------
    Chess(GameType g);
    Chess(GameType g, int ai);

    // No copy and move constructors
    Chess(const Chess& other);
    Chess(Chess&& other);

    Chess& operator=(const Chess& other);
    Chess& operator=(Chess&& other);

    /*
        Reset the board using the FEN string;
    */
    void build_from_fen(std::string fen);

    // Set AI difficulty between 1 - 10, can only be done on singleplayer games
    void set_ai_difficulty(int ai);
    bool make_move(Square orig, Square dest);

    // start from square 0
    std::vector<Piece> retrieve_board();
};

// Provides interface between Nodejs and C++
#endif
