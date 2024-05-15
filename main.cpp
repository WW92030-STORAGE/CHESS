#include <iostream>
#include "chess.h"
#include "genetic.h"

int main() {
    srand(time(0));
    
    std::vector<ChessAI> v;
    for (int i = 0; i < 32; i++) {
        ChessAI ai = Genetic::randomAI();
        v.push_back(Genetic::mutate(ai));
        std::cout << "X";
    }
    std::cout << "\n";
    
    for (int i = 0; i < 8; i++) {
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
    
    for (int i = 0; i < 16; i++) Genetic::test(res[0], ChessAI(), true);
    for (int i = 0; i < 16; i++) Genetic::test(ChessAI(), res[0], true);
}

// MOB 0.732270 RBN 0.197456 QDEF 0.431728 KMOB 0.658957 KDEF -1.119280 OO 0.675849 CHK 0.817688 CKMT 1000.000000 MCNT 0.039661