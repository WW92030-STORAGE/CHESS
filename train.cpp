#include <iostream>
#include "chess.h"
#include "genetic.h"

int main() {
    srand(time(0));
    
    std::vector<ChessAI> v;
    for (int i = 0; i < 64; i++) {
        ChessAI ai = Genetic::randomAI();
        v.push_back(Genetic::mutate(ai));
        std::cout << "X";
    }
    std::cout << "\n";
    
    for (int i = 0; i < 32; i++) {
        std::vector<ChessAI> res = Genetic::tournament(v, true);
        std::random_shuffle(res.begin(), res.end());
        
        for (auto i : res) std::cout << i.toString() << std::endl;
        
        v.clear();
        for (int i = 0; i < res.size() - 1; i += 2) {
            v.push_back(Genetic::mutate(Genetic::cross(res[i], res[i + 1])));
            v.push_back(Genetic::mutate(Genetic::cross(res[i], res[i + 1])));
            v.push_back(Genetic::mutate(Genetic::cross(res[i], res[i + 1])));
            v.push_back(Genetic::mutate(Genetic::cross(res[i], res[i + 1])));
        }
    }
    
    // Reduction
    std::vector<ChessAI> res;
    while (true) {
        res = Genetic::tournament(v, true);
        if (res.size() <= 1) break;
        std::random_shuffle(res.begin(), res.end());
    
        v.clear();
        for (int i = 0; i < res.size() - 1; i += 2) {
            v.push_back(Genetic::mutate(Genetic::cross(res[i], res[i + 1])));
            v.push_back(Genetic::mutate(Genetic::cross(res[i], res[i + 1])));
        }
    }
    
    std::cout << "FINAL MODEL " << res[0].toString() << "\n";

	std::cout << "PLAYING AS WHITE\n";
    
    for (int i = 0; i < 16; i++) std::cout << Genetic::test(res[0], ChessAI(), false) << " ";
	std::cout << "\n";

	std::cout << "PLAYING AS BLACK\n";
    for (int i = 0; i < 16; i++) std::cout << Genetic::test(ChessAI(), res[0], false) << " ";
}

// FINAL MODEL MOB 1.479965 RBN 1.521226 QDEF 1.372417 KMOB 1.713126 KDEF 0.699423 OO -1.074068 CHK 0.403394 CKMT 1000.000000 MCNT 0.121838