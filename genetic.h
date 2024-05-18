#ifndef GENETIC_H
#define GENETIC_H

#include "chess.h"
#include <map>
#include <cmath>
#include <climits>
#include <cfloat>
#include <iostream>
#include <algorithm>

// Genetic variation on Turochamp -- a heuristic based algorithm developed by Alan Turing. It works similarly to the heuristic Tetris algorithm in the TETRIS repo.

class ChessAI {
    public:
    // Instance variables are coefficients. The descriptions are what each coefficient is scaled by when computing the score.
    
    //                  X  X  P  N  B  R  Q  K
    double values[8] = {0, 0, 1, 3, 3, 5, 9, 1000};
    
    double mob = 1; // Mobility of pieces excluding kings and pawns
    double rbndef = 1; // RBN defense counter
    double qdef = 1; // Queen defense counter
    double kmob = 1; // King mobility excludes castles
    double kdef = -1; // If the king is undefended we deduct some points.
    double oo = 1; // Castling -- add 1 point if castling is possible, another if castling is a legal move right now or has been done just now.
    double chk = 1; // Checks (don't worry about checkmates for now)
    double ckmt = 1000; // Checkmate value that replaces the check value upon the threat of a mate
    double movecount = -0.01;
    
    ChessAI() {
        mob = 1;
        rbndef = 1;
        qdef = 1;
        kmob = 1;
        kdef = -1;
        oo = 1;
        chk = 1;
        ckmt = 1000;
        movecount = -0.01;
    }
    
    ChessAI(const ChessAI& other) {
        mob = other.mob;
        rbndef = other.rbndef;
        qdef = other.qdef;
        kmob = other.kmob;
        kdef = other.kdef;
        oo = other.oo;
        chk = other.chk;
        ckmt = other.ckmt;
        movecount = other.movecount;
    }
    
    ChessAI(int m, int r, int q, int km, int kd, int o, int ch, int cm, int mc) {
        mob = m;
        rbndef = r;
        qdef = q;
        kmob = km;
        kdef = -kd;
        oo = o;
        chk = ch;
        ckmt = cm;
        movecount = mc;
    }
    
    float getOneSidedScore(ChessGame game, bool verbose = false) {
        double material = 0;
        /*
        if (verbose) std::cout << "{" << game.captures.size() << "}";
        for (auto i : game.captures) {
            // if (verbose) std::cout << i.toString() << " ";
            material += values[i.getID()];
        }
        if (verbose) std::cout << "<<<\n";
        */

        for (int x = 0; x < 8; x++) {
            for (int y = 0; y < 8; y++) {
                ChessPiece piece = game.board[x][y];
                if (!piece.isEmpty() && piece.getColor() == game.sidetomove) material += values[piece.getID()];
            }
        }
                
        std::vector<std::pair<std::pair<int, int>, std::pair<int, int>>> legals = game.getAllLegalMoves();
        std::vector<std::pair<std::pair<int, int>, std::pair<int, int>>> defs = game.getDefenses();
        
        double mobs = 0;
        std::map<std::pair<int, int>, int> pmoves;
        for (auto p : legals) {
            ChessPiece source = game.board[p.first.first][p.first.second];
            if (source.isPawn() || source.isKing() || source.isEmpty()) continue;
            if (pmoves.find(p.first) == pmoves.end()) pmoves.insert({p.first, 0});
            (*(pmoves.find(p.first))).second++;
        }
        
        int kmobs = 0;
        for (auto p : legals) {
            ChessPiece source = game.board[p.first.first][p.first.second];
            if (source.isKing() && abs(p.second.second) != 2) kmobs++;
        }
        
        for (auto p : pmoves) mobs += std::sqrt((double)(p.second));
        
        int rbndefs = 0;
        int qdefs = 0;
        int kdefcnt = 0;
        for (auto p : defs) {
            ChessPiece source = game.board[p.second.first][p.second.second];
            if (source.isBishop() || source.isRook() || source.isKnight()) rbndefs++;
            if (source.isQueen()) qdefs++;
            if (source.isKing()) kdefcnt++;
        }
        
        int kdefs = 0;
        
        int you = (game.sidetomove) ? (1<<0) : (1<<1);
        
        if (kdefcnt == 0) {
            for (auto p : game.getAllPieces(you | (1<<7))) {
                int dx[8] = {00, 01, 01, 01, 00, -1, -1, -1};
                int dy[8] = {01, 01, 00, -1, -1, -1, 00, 01};
                for (int i = 0; i < 8; i++) {
                    for (int k = 1; k < 9; k++) {
                        std::pair<int, int> des = {p.file() + dx[i] * k, p.rank() + dy[i] * k};
                        if (!game.inBounds(des)) break;
                        ChessPiece x = game.board[des.first][des.second];
                        if (!x.isEmpty()) break;
                        kdefs++;
                    }
                }
            }
        }
        
        ChessGame game2(game);
        game2.sidetomove = !game.sidetomove;
        
        int checks = (game2.noChecks()) ? 0 : 1;
        if (game2.checkmate()) checks = ckmt;
        
        int oos = 0;
        
        if (verbose) std::cout << "AI ANALYSIS " << material << " " << mobs << " " << kmobs << " " << rbndefs << " " << qdefs << " " << kdefs << " " << oos << " " << checks << "\n";
        
        int movecnt = game.halfmoveclock;
        
        return material + mobs * mob + kmob * kmobs + rbndef * rbndefs + qdef * qdefs + kdef * kdefs + chk * checks - movecnt * movecount;
    }

    double getScore(ChessGame game, bool verbose = false) {
        double res = getOneSidedScore(game, verbose);
        ChessGame game2(game);
        game2.sidetomove = !game2.sidetomove;
        res -= getOneSidedScore(game2, verbose);
        return res;
    }
    
	// Maximizes your score after moving (opponent can do stuff later to lower it however)
    std::pair<std::pair<int, int>, std::pair<int, int>> pickdepth1(ChessGame game, bool verbose = false) {
        std::vector<std::pair<std::pair<int, int>, std::pair<int, int>>> legals = game.getAllLegalMoves();
        if (legals.size() == 0) return {std::make_pair(-1, -1), std::make_pair(0, 0)};
        
        std::pair<std::pair<int, int>, std::pair<int, int>> res = legals[0];
        double maxscore = -1 * DBL_MAX;
        for (auto p : legals) {
            if (verbose) std::cout << "[" << p.first.first << " " << p.first.second << " > " << p.second.first << " " << p.second.second << "]\n";
            ChessGame game2(game);
            game2.execute(p.first, p.second);
            if (verbose) for (auto i : game2.captures) std::cout << i.toString() << " ";
            if (verbose) std::cout << "---\n";
            double score = getScore(game2, verbose);
            if (score > maxscore) {
                maxscore = score;
                res = p;
            }
            if (score == maxscore) {
                if (rand() % 2 == 0) {
                    maxscore = score;
                    res = p;
                }
            }
        }
        return res;
    }

	// Minimizes the score of the opponent after moving
	std::pair<std::pair<int, int>, std::pair<int, int>> minoppd1(ChessGame game, bool verbose = false, int maxcons = 32) {
        std::vector<std::pair<std::pair<int, int>, std::pair<int, int>>> legals = game.getAllLegalMoves();
        if (legals.size() == 0) return {std::make_pair(-1, -1), std::make_pair(0, 0)};
        
        std::pair<std::pair<int, int>, std::pair<int, int>> res = legals[0];
        double maxscore = DBL_MAX;
        
        std::random_shuffle(legals.begin(), legals.end());
        
        for (int i = 0; i < maxcons && i < legals.size(); i++) {
            auto p = legals[i];
            if (verbose) std::cout << "[" << p.first.first << " " << p.first.second << " > " << p.second.first << " " << p.second.second << "]\n";
            ChessGame game2(game);
            game2.execute(p.first, p.second);
            if (verbose) for (auto i : game2.captures) std::cout << i.toString() << " ";
            if (verbose) std::cout << "---\n";
			game2.sidetomove = !game2.sidetomove;
            double score = getScore(game2, verbose);
            if (score < maxscore) {
                maxscore = score;
                res = p;
            }
            if (score == maxscore) {
                if (rand() % 2 == 0) {
                    maxscore = score;
                    res = p;
                }
            }
        }
        return res;
    }

	// Minimaxes the opponent's response (so basically it picks the move such that if the opponent responds in a way that gives you the worst outcome this worst outcome is lessened).

	std::pair<std::pair<int, int>, std::pair<int, int>> pickdepth2(ChessGame game, bool verbose = false, int maxcons = 32) {
		std::vector<std::pair<std::pair<int, int>, std::pair<int, int>>> legals = game.getAllLegalMoves();
		// for (auto i : legals) std::cout << "[" << i.first.first << " " << i.first.second << " " << i.second.first << " " << i.second.second << "]";
		// std::cout << "\n";
        if (legals.size() <= 0) {
            // std::cout << "PICK FAILED\n";
            return {std::make_pair(-1, -1), std::make_pair(0, 0)};
        }
        
        std::pair<std::pair<int, int>, std::pair<int, int>> res = legals[0];
        double maxscore = -1 * DBL_MAX;

		int x = 0;
		
		// std::cout << "PICKING MOVE...\n";
		
		// std::random_shuffle(legals.begin(), legals.end());
		for (int i = 0; i < legals.size() && i < maxcons; i++) {
		    // std::cout << i << " ";
			ChessGame game2(game);
			game2.execute(legals[i].first, legals[i].second);
			game2.sidetomove = !game2.sidetomove;
			auto oppmove = minoppd1(game2, verbose);
			game2.execute(oppmove.first, oppmove.second);
			game2.sidetomove = !game2.sidetomove;

			double score = getScore(game2, verbose);
			if (score >= maxscore) {
				maxscore = score;
				res = std::make_pair(legals[i].first, legals[i].second);
			}
			if (score == maxscore) {
				if (rand() % 2 == 0) res = std::make_pair(legals[i].first, legals[i].second);
			}
		}
		
		// std::cout << "\nMOVE PICKED\n";
		return res;
	}

    std::pair<std::pair<int, int>, std::pair<int, int>> chosenmove = {{0, 0}, {0, 0}};

    int leafcount = 0;

    double abprune(ChessGame game, int remlayers, double alpha, double beta, bool isMaximizing) { // remlayers must start (outermost call) at an even number
        if (remlayers <= 0) {
            leafcount++;
            return getScore(game);
        }

        if (isMaximizing) {
            double res = -1 * DBL_MAX;
            std::vector<std::pair<std::pair<int, int>, std::pair<int, int>>> legals = game.getAllLegalMoves();

            for (auto p : legals) {
                ChessGame game2(game);
                game2.execute(p.first, p.second);

                game2.sidetomove = !game2.sidetomove;
                double value = abprune(game2, remlayers - 1, alpha, beta, false);
                if (value > res) {
                    chosenmove = p;
                    res = value;
                }
                alpha = std::max(alpha, res);
                if (beta <= alpha) break;
            }

            return res;
            
        }

        else {
            double res = DBL_MAX;
            std::vector<std::pair<std::pair<int, int>, std::pair<int, int>>> legals = game.getAllLegalMoves();
            for (auto p : legals) {
                ChessGame game2(game);
                game2.execute(p.first, p.second);
                game2.sidetomove = !game2.sidetomove;

                double value = abprune(game2, remlayers - 1, alpha, beta, true);
                if (value < res) {
                    res = value;
                    // chosenmove = p;
                }
                beta = std::min(beta, res);
                if (beta <= alpha) break;
            }
            return res;
        }
        return -1;


    }

	std::pair<std::pair<int, int>, std::pair<int, int>> pick(ChessGame game, bool verbose = false) {
	    // return pickdepth2(game, false);

        leafcount = 0;
        chosenmove = game.getAllLegalMoves()[0];
        abprune(game, 2, -1 * DBL_MAX, DBL_MAX, true);
        std::cout << leafcount << " LEAF NODES CHECKED\n";
        return chosenmove;
	}
    
    // mob / rbndef / qdef / kmob / kdef / oo / chk / ckmt / movecount
    
    std::string toString() {
        std::string res = "MOB " + std::to_string(mob) + " RBN " + std::to_string(rbndef) + " QDEF " + std::to_string(qdef);
        res = res + " KMOB " + std::to_string(kmob) + " KDEF " + std::to_string(kdef) + " OO " + std::to_string(oo);
        res = res + " CHK " + std::to_string(chk) + " CKMT " + std::to_string(ckmt) + " MCNT " + std::to_string(movecount);
        return res;
    }
};

namespace Genetic {
    
// Play with a1 white and a2 black
int test(ChessAI a1, ChessAI a2, bool verbose = false) {
    ChessGame game;
    
    while (true) { // a1 white a2 black
        auto move = game.sidetomove ? (a1.pick(game)) : (a2.pick(game));
        game.execute(move.first, move.second);
        game.sidetomove = !game.sidetomove;
        
        if (verbose) std::cout << game.toString() << "\n";
            
        if (game.checkmate()) {
            if (verbose) std::cout << game.toString() << "\n";
            if (verbose) std::cout << ( game.sidetomove ? "BLACK" : "WHITE" ) << " WINS\n";
            return game.sidetomove ? (-1) : (1);
        }
        
        if (game.stalemate()) {
            if (verbose) std::cout << game.toString() << "\n";
            if (verbose) std::cout << "STALEMATE/DRAW\n";
            return 0;
        }
    }
}

std::vector<ChessAI> tournament(std::vector<ChessAI> ais, bool verbose = false) {
    std::random_shuffle(ais.begin(), ais.end());
    std::vector<ChessAI> res;
    for (int i = 0; i < ais.size() - 1; i += 2) {
        int val = test(ais[i], ais[i + 1]);
        if (val > 0) res.push_back(ChessAI(ais[i]));
        else if (val < 0) res.push_back(ChessAI(ais[i + 1]));
        else {
            if (rand() % 2 == 0) res.push_back(ChessAI(ais[i]));
            else res.push_back(ChessAI(ais[i + 1]));
        }
        if (verbose) std::cout << "X\n";
    }
    if (verbose) std::cout << "\n";
    return res;
}

// mob / rbndef / qdef / kmob / kdef / oo / chk / ckmt / movecount

ChessAI cross(ChessAI a1, ChessAI a2) {
    ChessAI res(a1);
    if (rand() % 2 == 0) res.mob = a2.mob;
    if (rand() % 2 == 0) res.rbndef = a2.rbndef;
    if (rand() % 2 == 0) res.qdef = a2.qdef;
    if (rand() % 2 == 0) res.kmob = a2.kmob;
    if (rand() % 2 == 0) res.kdef = a2.kdef;
    if (rand() % 2 == 0) res.oo = a2.oo;
    if (rand() % 2 == 0) res.chk = a2.chk;
    if (rand() % 2 == 0) res.ckmt = a2.ckmt;
    if (rand() % 2 == 0) res.movecount = a2.movecount;
    return res;
}

double randf() {
    return (double)(rand()) / RAND_MAX;
}

// mob / rbndef / qdef / kmob / kdef / oo / chk / ckmt / movecount

ChessAI mutate(ChessAI ai) {
    ChessAI res(ai);
    int beep = rand() % 64;
    if (beep == 0) res.mob = randf() * 4 - 2;
    if (beep == 1) res.rbndef = randf() * 4 - 2;
    if (beep == 2) res.qdef = randf() * 4 - 2;
    if (beep == 3) res.kmob = randf() * 4 - 2;
    if (beep == 4) res.kdef = randf() * -4 + 2;
    if (beep == 5) res.oo = randf() * 4 - 2;
    if (beep == 6) res.chk = randf() * 4 - 2;
    // if (beep == 7) res.ckmt = randf() * 400;
    if (beep == 8) res.movecount = (0.5 - randf()) * 0.5;
    return res;
}

ChessAI randomAI() {
    ChessAI res;
    res.mob = randf() * 4 - 2;
    res.rbndef = randf() * 4 - 2;
    res.qdef = randf() * 4 - 2;
    res.kmob = randf() * 4 - 2;
    res.kdef = randf() * -4 + 2;
    res.oo = randf() * 4 - 2;
    res.chk = randf() * 4 - 2;
    // if (beep == 7) res.ckmt = randf() * 400;
    res.movecount = (0.5 - randf()) * 0.5;
    return res;
}

}

#endif