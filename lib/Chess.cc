#include "Chess.h"

typedef unsigned int uint;


Chess::Chess() : Chess(Types::DEFAULT_FEN, 0) {

}

Chess::Chess(std::string fen) : Chess(fen, 0) {

}

Chess::Chess(std::string fen, int ai) {
    this->board = Board(fen);
}

Chess::Chess(const Chess& other) {
    this->board = other.board;
}

Chess::Chess(Chess&& other) {
    this->board = std::move(other.board);
}

Chess& Chess::operator=(const Chess& other) {
    this->board = other.board;
    return *this;
}

Chess& Chess::operator=(Chess&& other) {
    this->board = std::move(other.board);
    return *this;
}

bool Chess::make_move(Square orig, Colour col, Square dest) {

    // Player making the move

    // Determine move type
    MoveType type = Moves::infer_move_type(orig, col, dest, this->board);

    // Validate move
    bool is_valid = Moves::is_valid_move(orig, col, dest, type, this->board);

    if(is_valid) {
        return this->board.make_move(orig, dest, type);
    } else {
        return false;
    }
}
