#include <stdexcept>
#include <iostream>
#include "Board.h"

typedef unsigned int uint;

// ------ Constructors -------------
Board::Board() {
    std::cout << "Called Board empty constructor" << std::endl;
    this->boards[0]  = Bitboard(0x0000000000000010); // White King
    this->boards[1]  = Bitboard(0x0000000000000008); // White Queen
    this->boards[2]  = Bitboard(0x0000000000000024); // White Bishop
    this->boards[3]  = Bitboard(0x0000000000000042); // White Knight
    this->boards[4]  = Bitboard(0x0000000000000081); // White Rook
    this->boards[5]  = Bitboard(0x000000000000FF00); // White Pawn

    this->boards[6]  = Bitboard(0x1000000000000000); // Black King
    this->boards[7]  = Bitboard(0x0800000000000000); // Black Queen
    this->boards[8]  = Bitboard(0x2400000000000000); // Black Bishop
    this->boards[9]  = Bitboard(0x4200000000000000); // Black Knight
    this->boards[10] = Bitboard(0x8100000000000000); // Black Rook
    this->boards[11] = Bitboard(0x00FF000000000000); // Black Pawn

    this->current_turn = WHITE;
    this->w_castle_rights = 3;
    this->b_castle_rights = 3;
}

Board::Board(std::string fen) {
    std::cout << "Called Board FEN constructor with following value: " << fen << std::endl;
    this->build_from_fen(fen);
}

// Copy and move constructors
Board::Board(const Board& other) {
    for(uint i=0; i<12; i++) {
        this->boards[i] = other.boards[i];
    }

    this->w_castle_rights = other.w_castle_rights;
    this->b_castle_rights = other.b_castle_rights;
}

Board::Board(Board&& other) {
    for(uint i=0; i<12; i++) {
        this->boards[i] = other.boards[i];
    }
    this->w_castle_rights = other.w_castle_rights;
    this->b_castle_rights = other.b_castle_rights;
}

// Destructor, no dynamic memory
Board::~Board() {

}

// ----Copy and Move Operators ---------
Board& Board::operator=(const Board& other) {
    if(&other != this) {
        for(uint i=0; i<12; i++) {
            this->boards[i] = other.boards[i];
        }
        this->w_castle_rights = other.w_castle_rights;
        this->b_castle_rights = other.b_castle_rights;
    }
    return *this;
}


Board& Board::operator=(Board&& other) {
    for(uint i=0; i<12; i++) {
        this->boards[i] = other.boards[i];
    }
    this->w_castle_rights = other.w_castle_rights;
    this->b_castle_rights = other.b_castle_rights;
    return *this;
}

// ------------- Board Operators ---------------------
Piece Board::get_piece_at(Square s) const{
    uint64_t i = 1;
    uint64_t bb = i << s;
    for(int j=0; j<12; j++) {
        if((boards[j].bb & bb) > 0) {
            return (Piece)j;
        }
    }
    return PIECE_NONE;
}


Bitboard Board::operator[] (Piece p) const {
    if(p == PIECE_NONE) {
        uint64_t none_bb = 0;
        for(int i=0; i<12; i++) {
            none_bb |= this->boards[i].bb;
        }
        Bitboard bb(~none_bb);
        return bb;

    } else {
        unsigned int i = (unsigned int)p;
        return this->boards[i];
    }
}

Bitboard Board::all_white_bb() const {
    Bitboard bb;
    for(uint i=0; i<6; i++) {
        bb & this->boards[i];
    }
    return bb;
}

Bitboard Board::all_black_bb() const {
    Bitboard bb;
    for(uint i=6; i<12; i++) {
        bb & this->boards[i];
    }
    return bb;
}

bool Board::make_move(Square orig, Square dest, MoveType type) {
    Piece orig_p = this->get_piece_at(orig);
    Piece dest_p = this->get_piece_at(dest);
    (*this)[orig_p] ^= orig;
    (*this)[dest_p] |= dest;
    return true;
}





void Board::build_from_fen(std::string fen) {
    //std::regex reg("(([KQBNRPkqbnrp\\/1-8]+)(\\ [bw]\\ )(-|[KQkq]+\\ )(-|[a-h][1-8]\\ )(\\d\\ )(\\d))");
    std::regex reg("(([KQBNRPkqbnrp\\/1-8]+)(\\ [bw]\\ )(-|[KQkq]+\\ )(-\\ |[a-h][1-8]\\ )(\\d+\\ )(\\d+))");
    std::smatch matches;
    bool res = std::regex_match(fen, matches, reg);
    if(!res) {
        throw std::invalid_argument("Invalid FEN string");
    }

    // Extract string matches
    std::string b_str = matches[2];
    b_str = Types::remove_all_char(b_str, ' ');

    std::string turn_str = matches[3];
    turn_str = Types::remove_all_char(turn_str, ' ');

    std::string castle_str = matches[4];
    castle_str = Types::remove_all_char(castle_str, ' ');

    std::string enpassant_str = matches[5];
    enpassant_str = Types::remove_all_char(enpassant_str, ' ');

    std::string halfmove_str = matches[6];
    halfmove_str = Types::remove_all_char(halfmove_str, ' ');

    std::string fullmove_str = matches[7];
    fullmove_str = Types::remove_all_char(fullmove_str, ' ');

    // convert to correct types and overwrite
    // reset board string be 64chars long
    b_str = Types::remove_all_char(b_str, '/');
    b_str = Types::replace_all_char(b_str, '1', "_");
    b_str = Types::replace_all_char(b_str, '2', "__");
    b_str = Types::replace_all_char(b_str, '3', "___");
    b_str = Types::replace_all_char(b_str, '4', "____");
    b_str = Types::replace_all_char(b_str, '5', "_____");
    b_str = Types::replace_all_char(b_str, '6', "______");
    b_str = Types::replace_all_char(b_str, '7', "_______");
    b_str = Types::replace_all_char(b_str, '8', "________");

    std::cout << "Extracted board     string: " << b_str << std::endl;
    std::cout << "Extracted turn      string: " << turn_str << std::endl;
    std::cout << "Extracted castle    string: " << castle_str << std::endl;
    std::cout << "Extracted enpassant string: " << enpassant_str << std::endl;
    std::cout << "Extracted halfmove  string: " << halfmove_str << std::endl;
    std::cout << "Extracted fullmove  string: " << fullmove_str << std::endl;

    uint64_t black_rook = 0;
    uint64_t black_knight = 0;
    uint64_t black_bishop = 0;
    uint64_t black_queen = 0;
    uint64_t black_king = 0;
    uint64_t black_pawn = 0;
    uint64_t white_rook = 0;
    uint64_t white_knight = 0;
    uint64_t white_bishop = 0;
    uint64_t white_queen = 0;
    uint64_t white_king = 0;
    uint64_t white_pawn = 0;

    // find every piece and set its location in the boards array
    // 1 << index
    // i  b
    // 0  56
    // 1  57
    // 2  58
    // 3  59
    // 4  60
    // 5  61
    // 6  62
    // 7  63
    // 8  55
    int rank = 56;
    int file =  0;
    // rn_qkbnrpp__p_pp__p_________p_____BpP_Q___________PPPP_PPPRNB_K__R
    // rn_qkbnr pp__p_pp __p_____ ____p___ __BpP_Q_ ________ PPPP_PPP RNB_K__R'
    for(unsigned int i=0; i<b_str.size(); i++) {
        uint64_t one = 1;
        if( i % 8 == 0) {
            rank = 56 - (8*(i / 8));
        }
        file = (i % 8);
        if(b_str[i] == 'r') {
            black_rook |= (one << (rank + file));
            continue;
        }

        if(b_str[i] == 'n') {
            black_knight |= (one << (rank + file));
            continue;
        }

        if(b_str[i] == 'b') {
            black_bishop |= (one << (rank + file));
            continue;
        }
        if(b_str[i] == 'q') {
            black_queen |= (one << (rank + file));
            continue;
        }
        if(b_str[i] == 'k') {
            black_king |= (one << (rank + file));
            continue;
        }

        if(b_str[i] == 'p') {
            black_pawn |= (one << (rank + file));
            continue;
        }

        if(b_str[i] == 'R') {
            white_rook |= (1 << (rank + file));
            continue;
        }
        if(b_str[i] == 'N') {
            white_knight |= (one << (rank + file));
            continue;
        }
        if(b_str[i] == 'B') {
            white_bishop |= (one << (rank + file));
            continue;
        }
        if(b_str[i] == 'Q') {
            white_queen |= (one << (rank + file));
            continue;
        }
        if(b_str[i] == 'K') {
            white_king |= (one << (rank + file));
            continue;
        }
        if(b_str[i] == 'P') {
            white_pawn |= (one << (rank + file));
            continue;
        }
    }
    this->boards[0] = Bitboard(white_king);
    this->boards[1] = Bitboard(white_queen);
    this->boards[2] = Bitboard(white_bishop);
    this->boards[3] = Bitboard(white_knight);
    this->boards[4] = Bitboard(white_rook);
    this->boards[5] = Bitboard(white_pawn);

    this->boards[6] = Bitboard(black_king);
    this->boards[7] = Bitboard(black_queen);
    this->boards[8] = Bitboard(black_bishop);
    this->boards[9] = Bitboard(black_knight);
    this->boards[10] = Bitboard(black_rook);
    this->boards[11] = Bitboard(black_pawn);

    // parse turn
    if(turn_str == "w") {
        this->current_turn = WHITE;
    } else {
        this->current_turn = BLACK;
    }

    // reset castling rights
    this->w_castle_rights = 0;
    this->b_castle_rights = 0;

    if(castle_str != "_") {
        for(unsigned int i=0; i<castle_str.size(); i++) {
            if(castle_str[i] == 'K') {
                this->w_castle_rights |= 1;
            } else if(castle_str[i] == 'Q') {
                this->w_castle_rights |= 2;
            } else if(castle_str[i] == 'k') {
                this->b_castle_rights |= 1;
            } else if(castle_str[i] == 'q') {
                this->b_castle_rights |= 2;
            }
        }
    }
    // ignore enpassant and the rest for now
}
