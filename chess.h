#ifndef CHESS_H
#define CHESS_H

#include <iostream>
#include <vector>
#include <string>
#include <set>
#include <algorithm>

struct ChessPiece {
    char value;
    
    // WHITE BLACK ... PAWN KNIGHT BISHOP ROOK QUEEN KING
    char ids[8] = {'_', '*', 'P', 'N', 'B', 'R', 'Q', 'K'};
    
    ChessPiece() {
        value = 0;
    }
    
    ChessPiece(char c) {
        value = c;
    }
    
    ChessPiece(const ChessPiece& other) {
        value = other.value;
    }
    
    bool isWhite() { return (value & (1<<0)); }
    bool isBlack() { return (value & (1<<1)); }
    bool isPawn() { return (value & (1<<2)); }
    bool isKnight() { return (value & (1<<3)); }
    bool isBishop() { return (value & (1<<4)); }
    bool isRook() { return (value & (1<<5)); }
    bool isQueen() { return (value & (1<<6)); }
    bool isKing() { return (value & (1<<7)); }
    
    int getID() {
        for (int i = 2; i < 8; i++) {
            if (value & (1<<i)) return i;
        }
        return 0;
    }
    
    bool getColor() { // deal with it
        return isWhite();
    }
    
    bool isEmpty() {
        return value == 0;
    }
    
    bool isSlider() {
        return isPawn() || isBishop() || isRook() || isQueen() || isKing();
    }
    
    std::string toString() {
        if (value == 0) return "..";
        std::string res = "";
        for (int i = 0; i < 8; i++) {
            if (value & (1<<i)) res = res + ids[i];
        }
        if (res.length() > 2) res = res.substr(0, 2);
        while (res.length() < 2) res = res + "~";
        return res;
    }
    
    bool operator<(ChessPiece& other) { return value < other.value; }
    bool operator==(ChessPiece& other) { return value == other.value; }
    bool operator!=(ChessPiece& other) { return value != other.value; }
};

struct Position {
    // Grid cells are represented as chars -- lower 3 bits file, upper 3 bits rank.
    char value = -1;
    
    Position() { value = -1; }
    Position(int file, int rank) { value = file + 8 * rank; }
    Position(std::pair<int, int> p) { value = p.first + 8 * p.second; }
    
    /*
    Position(char file, int rank) {
        if (file >= 'a' && file <= 'z') file = file - 'a' + 'A';
        value = (file - 'A') + 8 * (rank - 1);
    }
    */
    
    Position(const Position& other) {
        value = other.value;
    }
    
    char file() { 
        char res = value % 8;
        while (res < 0) res += 8;
        return res;
    }
    char rank() { 
        char res = value / 8;
        while (res < 0) res += 8;
        return res % 8;
    }
    
    std::pair<int, int> pos() {
        return {(int)(file()), (int)(rank())};
    }
    
    std::string toString() {
        if (value == -1) return "..";
        std::string res(1, file() + 'a');
        return res + std::to_string((int)(rank() + 1));
    }
    
    std::pair<int, int> pp() {
        if (value == -1) return {-1, -1};
        return {file(), rank()};
    }
    
    bool operator<(Position& other) { return value < other.value; }
    bool operator==(Position& other) { return value == other.value; }
    bool operator!=(Position& other) { return value != other.value; }
};

struct ChessGame { // A chess game at some particular state
    // Side to move is a single bit -- true is WHITE
    bool sidetomove = true; // Slight misnomer - this value actually stores which side we are moving and analyzing. The turn formally changes when this value is rotated.
    
    // Various statistics are pairs [WHITE, BLACK]
    std::pair<bool, bool> castleq = {true, true};
    std::pair<bool, bool> castlek = {true, true};
    std::pair<char, char> eps = {-1, -1}; // Empty pair is always [-1, -1]. En passant always refers to files since from the file and side we can determine the rank.
    int halfmoveclock = 0; // Counts number of reversible moves since last event horizon
    std::vector<ChessPiece> captures; // Captured pieces on the current move
    
    int maxmoves = 100;
    
    ChessPiece board[8][8];
    
    
    ChessGame() {
        sidetomove = true;
        castleq = {true, true};
        castlek = {true, true};
        eps = {-1, -1};
        
        reset();
    }
    
    ChessGame(const ChessGame& other) {
        sidetomove = other.sidetomove;
        castleq = std::make_pair(other.castleq.first, other.castleq.second);
        castlek = std::make_pair(other.castlek.first, other.castlek.second);
        eps = {other.eps.first, other.eps.second};
        halfmoveclock = other.halfmoveclock;
        maxmoves = other.maxmoves;
        
        for (auto i : other.captures) captures.push_back(ChessPiece(i));
        
        for (int x = 0; x < 8; x++) {
            for (int y = 0; y < 8; y++) board[x][y] = ChessPiece(other.board[x][y]);
        }
    }
    
    char backrank[8] = {(1<<5), (1<<3), (1<<4), (1<<6), (char)(128), (1<<4), (1<<3), (1<<5)};
    
    void reset() {
        castleq = {true, true};
        castlek = {true, true};
        eps = {-1, -1};
        halfmoveclock = 0;
        
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) board[i][j] = ChessPiece(0);
        }
        
        for (int x = 0; x < 8; x++) {
            board[x][0] = ChessPiece(backrank[x] | (1<<0));
            board[x][7] = ChessPiece(backrank[x] | (1<<1));
            board[x][1] = ChessPiece((1<<2) | (1<<0));
            board[x][6] = ChessPiece((1<<2) | (1<<1));
        }
    }
    
    
    std::string toString() {
        std::string res = "";
        for (int y = 7; y >= 0; y--) {
            for (int x = 0; x < 8; x++) res = res + board[x][y].toString() + " ";
            res = res + "\n";
        }
        
        res = res + "(" + (sidetomove ? "WHITE" : "BLACK") + ")";
        res = res + "[" + std::to_string(castleq.first) + std::to_string(castlek.first) + "/" + std::to_string(castleq.second) + std::to_string(castlek.second) + "]";
        res = res + "[" + std::to_string(eps.first) + " " + std::to_string(eps.second) + "]";
        res = res + "<" + std::to_string(halfmoveclock) + ">\n";
        return res;
    }
    
    bool operator<(ChessGame& other) {
        if (sidetomove != other.sidetomove) return sidetomove < other.sidetomove;
        if (castleq != other.castleq) return castleq < other.castleq;
        if (castlek != other.castlek) return castlek < other.castlek;
        if (eps != other.eps) return eps < other.eps;
        if (halfmoveclock != other.halfmoveclock) return halfmoveclock < other.halfmoveclock;
        
        for (int i = 0; i < 64; i++) {
            if (board[i % 8][i / 8] != other.board[i % 8][i / 8]) return board[i % 8][i / 8] < other.board[i % 8][i / 8];
        }
        return 0;
    }

    bool sameState(ChessGame& other) {
        if (sidetomove != other.sidetomove) return false;
        if (castleq != other.castleq) return false;
        if (castlek != other.castlek) return false;
        /*
        if (eps != other.eps) return false;
        if (halfmoveclock != other.halfmoveclock) return false;
        */
        
        for (int i = 0; i < 64; i++) {
            if (board[i % 8][i / 8] != other.board[i % 8][i / 8]) return false;
        }
        return true;
    }
    
    // Piece movement and interaction
    
    std::vector<Position> getAllPieces(char value) {
        std::vector<Position> res;
        for (int x = 0; x < 8; x++) {
            for (int y = 0; y < 8; y++) if (board[x][y].value == value) res.push_back(Position(x, y));
        }
        return res;
    }
    
    bool inBounds(std::pair<int, int> p) {
        if (p.first < 0 || p.second < 0 || p.first > 7 || p.second > 7) return false;
        return true;
    }
    
    bool inBounds(int x, int y) {
        return inBounds(std::make_pair(x, y));
    }
    
    // Is the movement vector legal without regard to the surroundings? Does not regard castling as a valid move vector
    
    bool isLegalVector(ChessPiece piece, int dx, int dy) { 
        if (dx == 0 && dy == 0) return false; // You must move.
        if (piece.isKing()) return std::max(abs(dx), abs(dy)) <= 1; // Chebyshev distance
        if (piece.isRook()) return dx * dy == 0;
        if (piece.isBishop()) return abs(dx) == abs(dy);
        if (piece.isQueen()) return (dx * dy == 0) || (abs(dx) == abs(dy));
        if (piece.isKnight()) return abs(dx * dy) == 2;
        if (piece.isPawn()) {
            if (piece.isWhite()) return ( (dx == 0) && (dy == 1 || dy == 2) ) || ( (dy == 1) && (abs(dx) == 1) );
            if (piece.isBlack()) return ( (dx == 0) && (dy == -1 || dy == -2) ) || ( (dy == -1) && (abs(dx) == 1) );
        }
        return false; // what are you doing?
    }
    
    bool isLegalVector(ChessPiece piece, std::pair<int, int> vec) {
        return isLegalVector(piece, vec.first, vec.second);
    }
    
    // Sliding can only occur on the cardinal and ordinal directions.
    bool isLegalSliding(std::pair<int, int> s, std::pair<int, int> e) {
        if (!inBounds(s) || !inBounds(e)) return false;
        
        int dx = e.first - s.first;
        int dy = e.second - s.second;
        if ((dx * dy != 0) && (abs(dx) != abs(dy))) return false;
        if (dx == dy && dy == 0) return false;
        
        int bx = (dx > 0) ? 1 : (dx < 0 ? -1 : 0);
        int by = (dy > 0) ? 1 : (dy < 0 ? -1 : 0);
        
        int x = s.first + bx;
        int y = s.second + by;
        
        for (int i = 0; i < std::max(abs(dx), abs(dy)) - 1; i++) {
            if (!inBounds({x, y})) return false; // what the fuck are you doing???
            if (!board[x][y].isEmpty()) return false; 
            x += bx;
            y += by;
        }
        return board[x][y].isEmpty() || (board[x][y].getColor() != board[s.first][s.second].getColor()); // can capture opposing pieces
    }
    
    // Are there no checks for the given (sidetomove) player and the current state?
    bool noChecks(bool verbose = false) {
        int you = (sidetomove) ? (1<<0) : (1<<1);
        int opp = (sidetomove) ? (1<<1) : (1<<0);
        
        ChessPiece THISKING(you | (1<<7));
        
        if (verbose) {
            for (auto p : getAllPieces(you | (1<<7))) std::cout << "K" << p.toString() << "\n";
        }
        
        for (auto p : getAllPieces(opp | (1<<2))) { // Pawns
            int dy = (sidetomove) ? -1 : 1;
            int dx[2] = {-1, 1};
            for (int i = 0; i < 2; i++) {
                if (!inBounds(p.file() + dx[i], p.rank() + dy)) continue;
                if (board[p.file() + dx[i]][p.rank() + dy] == THISKING) return false;
            }
        }
        
        for (auto p : getAllPieces(opp | (1<<3))) { // Knights
            int dx[8] = {02, 01, -1, -2, -2, -1, 01, 02};
            int dy[8] = {01, 02, 02, 01, -1, -2, -2, -1};
            for (int i = 0; i < 8; i++) {
                if (!inBounds(p.file() + dx[i], p.rank() + dy[i])) continue;
                if (board[p.file() + dx[i]][p.rank() + dy[i]] == THISKING) return false;
            }
        }
        
        for (auto p : getAllPieces(opp | (1<<4))) { // Bishops
            int dx[4] = {01, 01, -1, -1};
            int dy[4] = {01, -1, 01, -1};
            for (int i = 0; i < 4; i++) {
                for (int k = 1; k < 9; k++) {
                    if (!inBounds(p.file() + dx[i] * k, p.rank() + dy[i] * k)) break;
                    if (board[p.file() + dx[i] * k][p.rank() + dy[i] * k] == THISKING) return false;
                    if (!board[p.file() + dx[i] * k][p.rank() + dy[i] * k].isEmpty()) break;
                }
            }
        }
        
        for (auto p : getAllPieces(opp | (1<<5))) { // Rooks
            int dx[4] = {00, 01, 00, -1};
            int dy[4] = {01, 00, -1, 00};
            for (int i = 0; i < 4; i++) {
                for (int k = 1; k < 9; k++) {
                    if (!inBounds(p.file() + dx[i] * k, p.rank() + dy[i] * k)) break;
                    if (board[p.file() + dx[i] * k][p.rank() + dy[i] * k] == THISKING) return false;
                    if (!board[p.file() + dx[i] * k][p.rank() + dy[i] * k].isEmpty()) break;
                }
            }
        }
        
        for (auto p : getAllPieces(opp | (1<<6))) { // Queens
            int dx[8] = {00, 01, 01, 01, 00, -1, -1, -1};
            int dy[8] = {01, 01, 00, -1, -1, -1, 00, 01};
            for (int i = 0; i < 8; i++) {
                for (int k = 1; k < 9; k++) {
                    if (verbose) std::cout << "{" << p.file() + dx[i] * k << " " << p.rank() + dy[i]  * k << "}\n";
                    if (!inBounds(p.file() + dx[i] * k, p.rank() + dy[i] * k)) break;
                    if (board[p.file() + dx[i] * k][p.rank() + dy[i] * k] == THISKING) return false;
                    if (!board[p.file() + dx[i] * k][p.rank() + dy[i] * k].isEmpty()) break;
                }
            }
        }
        
        for (auto p : getAllPieces(opp | (1<<7))) { // Kings
            int dx[8] = {00, 01, 01, 01, 00, -1, -1, -1};
            int dy[8] = {01, 01, 00, -1, -1, -1, 00, 01};
            for (int i = 0; i < 8; i++) {
                for (int k = 1; k <= 1; k++) {
                    if (!inBounds(p.file() + dx[i] * k, p.rank() + dy[i] * k)) break;
                    if (board[p.file() + dx[i] * k][p.rank() + dy[i] * k] == THISKING) return false;
                    if (!board[p.file() + dx[i] * k][p.rank() + dy[i] * k].isEmpty()) break;
                }
            }
        }
        
        return true;
    }
    
    // Attempts a move and returns if it is pseudolegal.
    bool pseudolegal(std::pair<int, int> src, std::pair<int, int> vec, bool verbose = false) {
        
        std::pair<int, int> des = {src.first + vec.first, src.second + vec.second};
        if (!inBounds(des)) return false;
        
        ChessPiece piece = board[src.first][src.second];
        ChessPiece victim = board[des.first][des.second];
        if (piece.isEmpty()) return false;
        if (piece.getColor() != sidetomove) return false;
        
        if (piece.isKing()) { // CASTLING - This does a complete check to keep everything in one place.
            if (vec == std::make_pair(2, 0)) { // Kingside - towards the H column.
                if (sidetomove) {
                    if (!castlek.first) return false;
                    if (!board[5][0].isEmpty()) return false;
                    if (!board[6][0].isEmpty()) return false;
                    
                    ChessGame game(*this);
                    game.board[7][0] = ChessPiece(); // Kill off the rook there.
                    game.execute({4, 0}, {1, 0});
                    if (!game.noChecks()) return false;
                    game.execute({5, 0}, {1, 0});
                    if (!game.noChecks()) return false;
                }
                else {
                    if (!castlek.second) return false;
                    if (!board[5][7].isEmpty()) return false;
                    if (!board[6][7].isEmpty()) return false;
                    
                    ChessGame game(*this);
                    game.board[0][0] = ChessPiece(); // Kill off the rook there.
                    game.execute({4, 0}, {-1, 0});
                    if (!game.noChecks()) return false;
                    game.execute({3, 0}, {-1, 0});
                    if (!game.noChecks()) return false;
                }
                return true;
            }
            else if (vec == std::make_pair(-2, 0)) {
                if (sidetomove) {
                    if (!castleq.first) return false;
                    if (!board[1][0].isEmpty()) return false;
                    if (!board[2][0].isEmpty()) return false;
                    if (!board[3][0].isEmpty()) return false;
                    
                    ChessGame game(*this);
                    game.board[7][7] = ChessPiece(); // Kill off the rook there.
                    game.execute({4, 7}, {1, 0});
                    if (!game.noChecks()) return false;
                    game.execute({5, 7}, {1, 0});
                    if (!game.noChecks()) return false;
                }
                else {
                    if (!castleq.second) return false;
                    if (!board[1][7].isEmpty()) return false;
                    if (!board[2][7].isEmpty()) return false;
                    if (!board[3][7].isEmpty()) return false;
                    
                    ChessGame game(*this);
                    game.board[0][7] = ChessPiece(); // Kill off the rook there.
                    game.execute({4, 7}, {-1, 0});
                    if (!game.noChecks()) return false;
                    game.execute({3, 7}, {-1, 0});
                    if (!game.noChecks()) return false;
                }
                return true;
            }
        }
        
        if (!isLegalVector(piece, vec)) return false;
        if (piece.isPawn()) {
            if (sidetomove) {
                if (src.second != 1 && abs(vec.second) == 2) return false;
            }
            else {
                if (src.second != 6 && abs(vec.second) == 2) return false;
            }
        }
        
        if (piece.isSlider()) {
            if (!isLegalSliding(src, des)) return false;
        }
        
        if (!victim.isEmpty() && victim.getColor() == piece.getColor()) return false;
        bool capture = (piece.getColor() != victim.getColor()) && !piece.isEmpty() && !victim.isEmpty();
        if (capture && victim.isKing()) return false; // Cannot capture king directly
        
        // Pawns that move forwards cannot capture
        
        if (piece.isPawn() && vec.first == 0) {
            if (!victim.isEmpty()) return false;
        }
        
        // Pawn capturing check
        
        Position enpassant;
        if (eps.first >= 0) enpassant = Position(eps.first, 2);
        if (eps.second >= 0) enpassant = Position(eps.second, 5);
        
        if (verbose) std::cout << enpassant.pp().first << " " << enpassant.pp().second << "<<\n";
        
        if (piece.isPawn()) {
            if (abs(vec.first) == 1) {
                if (victim.isEmpty() && enpassant.value != -1 && enpassant.pp() == des) {
                    // Capture EP
                    return true;
                }
                if (capture) {
                    // Capture NON-EP
                    eps.first = -1;
                    eps.second = -1;
                    return true;
                }
                if (victim.isEmpty()) return false;
            }
        }
        
        // En passant square -- reset automatically
        
        eps.first = -1;
        eps.second = -1;
        
        // Mark the relevant file. From this value and its place in the pair we can determine where the ep happens.
        
        if (piece.isPawn() && (abs(vec.second) == 2)) {
            if (sidetomove) eps.first = src.first;
            else eps.second = src.first;
        }
        
        // All other captures and moves
        
        return true;
    }
    
    // Moves a piece regardless of legality. If certain conditions are met (e.g. enpassant, castling) those actions are taken.
    void execute(std::pair<int, int> src, std::pair<int, int> vec, bool verbose = false) {
        captures.clear();
        std::pair<int, int> des = {src.first + vec.first, src.second + vec.second};
        ChessPiece temp = board[src.first][src.second];
        
        // Update castling rights
        
        if (temp.isKing()) {
            if (sidetomove) castleq.first = castlek.first = false;
            else castleq.second = castlek.second = false;
        }
        
        if (temp.isRook() && src.first == 0) {
            if (sidetomove) castleq.first = false;
            else castleq.second = false;
        }
        
        if (temp.isRook() && src.first == 7) {
            if (sidetomove) castlek.first = false;
            else castlek.second = false;
        }
        
        // If the king moves 2 cells horizontally we assume castle and the corresponding cell moves inversely.
        if (temp.isKing()) {
            if (vec == std::make_pair(-2, 0)) {
                if (sidetomove) {
                    board[0][0] = ChessPiece();
                    board[4][0] = ChessPiece();
                    board[2][0] = ChessPiece((1<<7) | (1<<0));
                    board[3][0] = ChessPiece((1<<5) | (1<<0));
                }
                else {
                    board[0][7] = ChessPiece();
                    board[4][7] = ChessPiece();
                    board[2][7] = ChessPiece((1<<7) | (1<<1));
                    board[3][7] = ChessPiece((1<<5) | (1<<1));
                }
                halfmoveclock++;
                return;
            }
            if (vec == std::make_pair(2, 0)) {
                if (sidetomove) {
                    board[7][0] = ChessPiece();
                    board[4][0] = ChessPiece();
                    board[6][0] = ChessPiece((1<<7) | (1<<0));
                    board[5][0] = ChessPiece((1<<5) | (1<<0));
                }
                else {
                    board[7][7] = ChessPiece();
                    board[4][7] = ChessPiece();
                    board[6][7] = ChessPiece((1<<7) | (1<<1));
                    board[5][7] = ChessPiece((1<<5) | (1<<1));
                }
                halfmoveclock++;
                return;
            }
        }
        
        if (!board[des.first][des.second].isEmpty()) halfmoveclock = 0;
        else if (temp.isPawn()) halfmoveclock = 0;
        else halfmoveclock++;
        board[src.first][src.second] = ChessPiece();
        
        if (!board[des.first][des.second].isEmpty()) {
            if (verbose) std::cout << des.first << " " << des.second << board[des.first][des.second].toString() << " CAPTURED\n";
            captures.push_back(ChessPiece(board[des.first][des.second]));
        }
        
        board[des.first][des.second] = temp;
        
        Position enpassant;
        if (eps.first >= 0) enpassant = Position(eps.first, 2);
        if (eps.second >= 0) enpassant = Position(eps.second, 5);
        
        // std::cout << enpassant.pp().first << " " << enpassant.pp().second << ">>\n";
        if (verbose) std::cout << des.first << " " << des.second << ">>\n";
        if (verbose) std::cout << enpassant.toString() << ">>\n";
        
        if (enpassant.pp() == des) {
            if (sidetomove) {
                captures.push_back(board[des.first][des.second - 1]);
                board[des.first][des.second - 1] = ChessPiece();
            }
            else {
                captures.push_back(board[des.first][des.second + 1]);
                board[des.first][des.second + 1] = ChessPiece();
            }
        }
        
        if (temp.isPawn()) {
            int you = (sidetomove) ? (1<<0) : (1<<1);
            if (sidetomove && des.second == 7) board[des.first][des.second] = ChessPiece(you | (1<<6));
            else if (!sidetomove && des.second == 0) board[des.first][des.second] = ChessPiece(you | (1<<6));
        }
    }
    
    bool legal(std::pair<int, int> src, std::pair<int, int> vec, bool verbose = false) {
        std::pair<int, int> des = {src.first + vec.first, src.second + vec.second};
        if (!pseudolegal(src, vec)) return false;
        
        // Test the move
        
        ChessGame game(*this);
        game.execute(src, vec);
        
        if (verbose) std::cout << toString() << "\n" << game.toString() << "\n";
        
        // Brute force all possible checking checks
        
        return game.noChecks(verbose);
    }
    
    std::vector<std::pair<std::pair<int, int>, std::pair<int, int>>> getAllLegalMoves(bool verbose = false) {
        std::vector<std::pair<std::pair<int, int>, std::pair<int, int>>> res;
        
        int you = (sidetomove) ? (1<<0) : (1<<1);
        
        ChessPiece THISKING(you | (1<<7));
        
        if (verbose) {
            for (auto p : getAllPieces(you | (1<<7))) std::cout << "K" << p.toString() << "\n";
        }
        
        for (auto p : getAllPieces(you | (1<<2))) { // Pawns
            int dy = (sidetomove) ? 1 : -1;
            int dx[2] = {-1, 1};
            for (int i = 1; i <= 2; i++) {
                std::pair<int, int> vec = {0, dy * i};
                if (legal(p.pos(), vec)) res.push_back({p.pos(), vec});
                
                vec = {dx[i], dy};
                if (legal(p.pos(), vec)) res.push_back({p.pos(), vec});
            }
        }
        
        for (auto p : getAllPieces(you | (1<<3))) { // Knights
            int dx[8] = {02, 01, -1, -2, -2, -1, 01, 02};
            int dy[8] = {01, 02, 02, 01, -1, -2, -2, -1};
            for (int i = 0; i < 8; i++) {
                std::pair<int, int> vec = {dx[i], dy[i]};
                if (legal(p.pos(), vec)) res.push_back({p.pos(), vec});
            }
        }
        
        for (auto p : getAllPieces(you | (1<<4))) { // Bishops
            int dx[4] = {01, 01, -1, -1};
            int dy[4] = {01, -1, 01, -1};
            for (int i = 0; i < 4; i++) {
                for (int k = 1; k < 9; k++) {
                    std::pair<int, int> vec = {dx[i] * k, dy[i] * k};
                    if (legal(p.pos(), vec)) res.push_back({p.pos(), vec});
                }
            }
        }
        
        for (auto p : getAllPieces(you | (1<<5))) { // Rooks
            int dx[4] = {00, 01, 00, -1};
            int dy[4] = {01, 00, -1, 00};
            for (int i = 0; i < 4; i++) {
                for (int k = 1; k < 9; k++) {
                    std::pair<int, int> vec = {dx[i] * k, dy[i] * k};
                    if (legal(p.pos(), vec)) res.push_back({p.pos(), vec});
                }
            }
        }
        
        for (auto p : getAllPieces(you | (1<<6))) { // Queens
            int dx[8] = {00, 01, 01, 01, 00, -1, -1, -1};
            int dy[8] = {01, 01, 00, -1, -1, -1, 00, 01};
            for (int i = 0; i < 8; i++) {
                for (int k = 1; k < 9; k++) {
                    std::pair<int, int> vec = {dx[i] * k, dy[i] * k};
                    if (legal(p.pos(), vec)) res.push_back({p.pos(), vec});
                }
            }
        }
        
        for (auto p : getAllPieces(you | (1<<7))) { // Kings
            int dx[8] = {00, 01, 01, 01, 00, -1, -1, -1};
            int dy[8] = {01, 01, 00, -1, -1, -1, 00, 01};
            for (int i = 0; i < 8; i++) {
                for (int k = 1; k <= 1; k++) {
                    std::pair<int, int> vec = {dx[i] * k, dy[i] * k};
                    if (legal(p.pos(), vec)) res.push_back({p.pos(), vec});
                }
            }
        }
        
        return res;
    }
    
    // Get all instances where a piece can capture another piece of the same color if said piece was the opposing color.
    std::vector<std::pair<std::pair<int, int>, std::pair<int, int>>> getDefenses(bool verbose = false) {
        std::vector<std::pair<std::pair<int, int>, std::pair<int, int>>> res;
        
        int you = (sidetomove) ? (1<<0) : (1<<1);
        
        ChessPiece THISKING(you | (1<<7));
        
        if (verbose) {
            for (auto p : getAllPieces(you | (1<<7))) std::cout << "K" << p.toString() << "\n";
        }
        
        for (auto p : getAllPieces(you | (1<<2))) { // Pawns
            int dy = (sidetomove) ? 1 : -1;
            int dx[2] = {-1, 1};
            for (int i = 1; i <= 2; i++) {
                std::pair<int, int> vec = {dx[i], dy};
                std::pair<int, int> des = {p.pos().first + vec.first, p.pos().second + vec.second};
                if (inBounds(des)) {
                    ChessPiece victim = board[des.first][des.second];
                    if (!victim.isEmpty() && victim.getColor() == sidetomove) res.push_back({p.pos(), des});
                }
            }
        }
        
        for (auto p : getAllPieces(you | (1<<3))) { // Knights
            int dx[8] = {02, 01, -1, -2, -2, -1, 01, 02};
            int dy[8] = {01, 02, 02, 01, -1, -2, -2, -1};
            for (int i = 0; i < 8; i++) {
                std::pair<int, int> vec = {dx[i], dy[i]};
                std::pair<int, int> des = {p.pos().first + vec.first, p.pos().second + vec.second};
                if (inBounds(des)) {
                    ChessPiece victim = board[des.first][des.second];
                    if (!victim.isEmpty() && victim.getColor() == sidetomove) res.push_back({p.pos(), des});
                }
            }
        }
        
        for (auto p : getAllPieces(you | (1<<4))) { // Bishops
            int dx[4] = {01, 01, -1, -1};
            int dy[4] = {01, -1, 01, -1};
            for (int i = 0; i < 4; i++) {
                for (int k = 1; k < 9; k++) {
                    std::pair<int, int> vec = {dx[i] * k, dy[i] * k};
                    std::pair<int, int> des = {p.pos().first + vec.first, p.pos().second + vec.second};
                    if (inBounds(des)) {
                        ChessPiece victim = board[des.first][des.second];
                        if (!victim.isEmpty() && victim.getColor() == sidetomove) {
                            res.push_back({p.pos(), des});
                            break; // Only one for sliding
                        }
                    }
                }
            }
        }
        
        for (auto p : getAllPieces(you | (1<<5))) { // Rooks
            int dx[4] = {00, 01, 00, -1};
            int dy[4] = {01, 00, -1, 00};
            for (int i = 0; i < 4; i++) {
                for (int k = 1; k < 9; k++) {
                    std::pair<int, int> vec = {dx[i] * k, dy[i] * k};
                    std::pair<int, int> des = {p.pos().first + vec.first, p.pos().second + vec.second};
                    if (inBounds(des)) {
                        ChessPiece victim = board[des.first][des.second];
                        if (!victim.isEmpty() && victim.getColor() == sidetomove) {
                            res.push_back({p.pos(), des});
                            break; // Only one for sliding
                        }
                    }
                }
            }
        }
        
        for (auto p : getAllPieces(you | (1<<6))) { // Queens
            int dx[8] = {00, 01, 01, 01, 00, -1, -1, -1};
            int dy[8] = {01, 01, 00, -1, -1, -1, 00, 01};
            for (int i = 0; i < 8; i++) {
                for (int k = 1; k < 9; k++) {
                    std::pair<int, int> vec = {dx[i] * k, dy[i] * k};
                    std::pair<int, int> des = {p.pos().first + vec.first, p.pos().second + vec.second};
                    if (inBounds(des)) {
                        ChessPiece victim = board[des.first][des.second];
                        if (!victim.isEmpty() && victim.getColor() == sidetomove) {
                            res.push_back({p.pos(), des});
                            break; // Only one for sliding
                        }
                    }
                }
            }
        }
        
        for (auto p : getAllPieces(you | (1<<7))) { // Kings
            int dx[8] = {00, 01, 01, 01, 00, -1, -1, -1};
            int dy[8] = {01, 01, 00, -1, -1, -1, 00, 01};
            for (int i = 0; i < 8; i++) {
                for (int k = 1; k <= 1; k++) {
                    std::pair<int, int> vec = {dx[i] * k, dy[i] * k};
                    std::pair<int, int> des = {p.pos().first + vec.first, p.pos().second + vec.second};
                    if (inBounds(des)) {
                        ChessPiece victim = board[des.first][des.second];
                        if (!victim.isEmpty() && victim.getColor() == sidetomove) {
                            res.push_back({p.pos(), des});
                        }
                    }
                }
            }
        }
        
        // std::vector<std::pair<std::pair<int, int>, std::pair<int, int>>> res2;
        // for (auto p : res) res2.push_back(std::make_pair(std::make_pair(p.first.first, p.first.second), std::make_pair(p.second.first, p.second.second)));
        
        return res;
    }
    
    std::string dispLegals() {
        std::string res = "LEGAL MOVES FOR ";
        res = res + (sidetomove ? "WHITE" : "BLACK") + "\n";
        std::vector<std::pair<std::pair<int, int>, std::pair<int, int>>> v = getAllLegalMoves();
        for (auto i : v) {
            res = res + Position(i.first).toString() + " [" + std::to_string(i.second.first) + ", " + std::to_string(i.second.second) + "]\n";
        }
        return res;
    }
    
    bool checkmate() { return getAllLegalMoves().size() == 0 && !noChecks(); }
    bool TLE() { return halfmoveclock >= maxmoves; }
    bool stalemate() { return TLE() || (getAllLegalMoves().size() == 0 && noChecks()); }
    bool gameover() { return checkmate() || stalemate(); }
};

#endif
