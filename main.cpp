#include <iostream>
#include "chess.h"
#include "genetic.h"

int main() {
    srand(time(0));

	ChessAI res(1.479965, 1.521226, 1.372417, 1.713126, 0.699423, -1.074068, 0.403394, 1000.000000, 0.121838);

	std::cout << "AS WHITE\n";
	for (int i = 0; i < 64; i++) std::cout << Genetic::test(res, Genetic::randomAI()) << " ";
	std::cout << "\n";

	std::cout << "AS BLACK\n";
	for (int i = 0; i < 64; i++) std::cout << Genetic::test(Genetic::randomAI(), res) << " ";
	std::cout << "\n";
}

// FINAL MODEL MOB 1.479965 RBN 1.521226 QDEF 1.372417 KMOB 1.713126 KDEF 0.699423 OO -1.074068 CHK 0.403394 CKMT 1000.000000 MCNT 0.121838