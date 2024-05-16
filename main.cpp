#include <iostream>
#include "chess.h"
#include "genetic.h"

int main() {
    srand(time(0));
	
	ChessAI res(1.814814, 1.912351, 1.191015, 1.137913, -1.492660, -1.201758, -1.734001, 1000.000000, -0.245666);

	std::cout << "AS WHITE\n";
	for (int i = 0; i < 128; i++) std::cout << Genetic::test(res, Genetic::randomAI()) << " ";
	std::cout << "\n";

	std::cout << "AS BLACK\n";
	for (int i = 0; i < 128; i++) std::cout << Genetic::test(Genetic::randomAI(), res) << " ";
	std::cout << "\n";
}

// moves = 1000 / depth = 1 / maxgain (1.479965, 1.521226, 1.372417, 1.713126, 0.699423, -1.074068, 0.403394, 1000.000000, 0.121838)
// moves = 100 / depth = 1 / maxgain (0.840907, 1.467513, 0.964812, 1.088595, -0.242012, 1.491317, 0.568194, 1000.000000, 0.149213)
// moves = 100 / depth = 1 / minopp (1.814814, 1.912351, 1.191015, 1.137913, -1.492660, -1.201758, -1.734001, 1000.000000, -0.245666)
