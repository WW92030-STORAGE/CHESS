#include <iostream>
#include "chess.h"

// INPUT is starting square then ending square e.g. on the first input "d2 d4" (no quotes) represents the move 1. e4.
// See below for some example test cases.

int main()
{
    
    ChessGame game;
    
    int sy, ey;
    char sx, ex;
    std::cout << game.toString() << "\n";
    while (std::cin >> sx >> sy >> ex >> ey) {
        if (sx >= 'A' && sx <= 'Z') sx = sx - 'A' + 'a';
        if (ex >= 'A' && ex <= 'Z') ex = ex - 'A' + 'a';
        sx = sx - 'a';
        sy--;
        ex = ex - 'a';
        ey--;
        if (!game.legal({sx, sy}, {ex - sx, ey - sy}, false)) {
            std::cout << "INVALID MOVE\n";
            continue;
        }
        
        game.execute({sx, sy}, {ex - sx, ey - sy}); // d2 d4 e7 e5 d4 d5 d8 h4 (f2 f4) <-- check example (last move is invalid since it exposes the king on e1 to the queen on h4)
        // e2 e4 e7 e5 f1 e2 f8 e7 g1 f3 g8 h6 e1 g1 <-- castle example
        
        std::cout << game.toString() << "\n";
        
        game.sidetomove = !game.sidetomove;
        if (game.checkmate()) { // f2 f4 e7 e5 g2 g4 d8 h4 <-- checkmate
            std::cout << "PROTOGEN PROTOGEN PROTOGEN PROTOGEN\n";
            return 0;
        }
        
        
        // std::cout << game.dispLegals();
    }

    return 0;
}