#include <iostream>
#include "chess.h"
#include "genetic.h"

void scoretable() {
	int black = 0;
	int white = 0;
	int x;
	while (std::cin >> x) {
	    if (x == -1000) break;
		if (x == -1) black++;
		if (x == 1) white++;
	}
	std::cout << "WHITE " << white << " BLACK " << black << std::endl;
}

// Example thing to run tournaments on engines. This instance runs one trained model on randomly generated models.

int main() {
    srand(time(0));

	ChessAI res(1.902865, 1.453464, 1.325278, 1.929444, -0.434348, -0.908216, 0.230627, 1000.000000, 0.110909); // Example engine

    int wb = 0;
    int bb = 0;
    int dr = 0;

	for (int i = 0; i < 4; i++) {
        int val = Genetic::test(res, Genetic::randomAI(), true);
        if (val > 0) wb++;
        if (val < 0) bb++;
        if (val == 0) dr++;
        // std::cout << val << " ";
    }
	std::cout << wb << " " << bb << " | " << dr << "\n";

	return 0;
}

// moves = maximum number of idle moves (non-pawn or capture) until resignation
// depth = how many moves ahead (including current incoming move) the model looks ahead
// maxgain = aims to maximize score for your side after your move / minopp = aims to minimize score for opponent after your move.

// moves = 100 / depth = 1 / minopp (0.123330, 0.265831, 0.897112, -0.111321, -0.363924, -1.067507, 1.903415, 1000.000000, -0.050465)
// moves = 100 / depth = 1 / minopp (0.109726, -0.780763, 1.211751, 0.962076, 0.007741, 0.326344, 0.019265, 1000.000000, -0.007614)

// OLD VER. (use genetic_OLD.h)

// moves = 1000 / depth = 1 / maxgain (1.479965, 1.521226, 1.372417, 1.713126, 0.699423, -1.074068, 0.403394, 1000.000000, 0.121838)
// moves = 100 / depth = 1 / maxgain (0.840907, 1.467513, 0.964812, 1.088595, -0.242012, 1.491317, 0.568194, 1000.000000, 0.149213)
// moves = 100 / depth = 1 / minopp (1.814814, 1.912351, 1.191015, 1.137913, -1.492660, -1.201758, -1.734001, 1000.000000, -0.245666)
