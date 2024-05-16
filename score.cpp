#include <iostream>
using namespace std;

// Calculates the white:black score table from a set of outputs of Genetic::test(...) (1 = white wins / 0 = draw / -1 = black wins)

int main() {
	int black = 0;
	int white = 0;
	int x;
	while (cin >> x) {
		if (x == -1) black++;
		if (x == 1) white++;
	}
	cout << "WHITE " << white << " BLACK " << black << endl;
}

/*

1 1 0 0 1 -1 0 0 1 0 1 -1 -1 1 -1 0 0 1 1 0 1 1 1 1 0 0 -1 1 0 -1 0 1 -1 0 1 0 1 1 1 1 0 0 1 -1 1 -1 0 1 1 -1 0 -1 0 1 0 1 0 1 0 0 -1 0 1 0 0 1 0 -1 1 0 1 0 1 0 0 0 1 -1 1 0 -1 1 -1 1 0 0 0 0 0 0 1 1 0 0 1 0 -1 0 0 -1 -1 1 0 1 0 1 0 -1 -1 1 0 0 -1 0 -1 -1 -1 1 1 1 1 -1 1 1 -1 1 0 1 

1 0 -1 0 0 0 1 1 -1 1 1 0 0 0 1 -1 -1 1 -1 -1 -1 -1 -1 -1 -1 -1 -1 0 0 0 0 -1 -1 -1 1 -1 -1 -1 1 0 0 0 1 -1 0 0 -1 0 0 -1 -1 0 0 -1 0 0 1 -1 0 -1 0 -1 0 -1 -1 -1 0 -1 0 1 -1 -1 -1 -1 -1 0 0 1 0 1 1 0 -1 -1 -1 1 -1 -1 -1 -1 -1 0 1 -1 0 -1 0 -1 0 0 0 -1 -1 -1 -1 -1 0 -1 -1 -1 -1 -1 -1 -1 -1 0 -1 1 1 -1 0 0 0 0 0 -1 0 0 

*/