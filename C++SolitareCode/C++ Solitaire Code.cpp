

#include <iostream>
#include<random>
#include "DummyPlayer.h"
#include"BoardScoreOptimiser.h"

void AIPlay(int numGames,int dealType) {

	int winCount = 0;
	float s[5] = { 10.818811,1.636655,6.108121,0.444798,6.098790 };
	BoardScorer bs = BoardScorer(s);
	for (int i = 0; i < numGames; i++) {
		DummyPlayer game(bs, dealType);
		//game = *new DummyPlayer(bs,dealType);
		std::cout << i << "/" << numGames << " \r";
		std::cout.flush();
		while (!game.isGameOver()) {
			game.makeBestMove();
		}
		if (game.gameWon()) {
			winCount++;
		}
	}
	std::cout << "Playing the immediate Best Move resulsts in" << endl;
	std::cout << "Games Won: " << winCount << endl;
	std::cout << "This is a Win rate of:" << (winCount / (float)numGames) * 100 << "%" << endl << endl;
	winCount = 0;
	for (int i = 0; i < numGames; i++) {
		std::cout << i << "/" << numGames << " \r";
		std::cout.flush();
		DummyPlayer game(bs,dealType);
		//game = DummyPlayer(dealType);
		while (!game.isGameOver()) {
			game.makeRandomMove();
		}
		if (game.gameWon()) {
			winCount++;
		}
	}
	std::cout << "Playing the Random Moves resulsts in" << endl;
	std::cout << "Games Won: " << winCount << endl;
	std::cout << "This is a Win rate of:" << (winCount / (float)numGames) * 100 << "%" << endl << endl;

	winCount = 0;
	for (int i = 0; i < numGames; i++) {
		std::cout << i << "/" << numGames << " \r";
		std::cout.flush();
		DummyPlayer game;
		game = DummyPlayer(bs,dealType);
		while (!game.isGameOver()) {
			game.suidRandomMove();
		}
		if (game.gameWon()) {
			winCount++;
		}
	}
	std::cout << "Playing the Suid Random Moves resulsts in" << endl;
	std::cout << "Games Won: " << winCount << endl;
	std::cout << "This is a Win rate of:" << (winCount / (float)numGames) * 100 << "%" << endl;
}


void player() {
	KlondikeGameController game = KlondikeGameController();
	while (game.isGameOver()){
		for (const auto& i : game.getAvailableMoves()) {
			std::cout << game.moveToString(i)<<endl;
		}
		int a, b, c, d;
		std::cout << "Give action 0-Draw,1-colToCol,2-colToFoun,3-DrawToFoun,4-FounToCol,5-DrawToCol" << endl;
		std::cin >> a;
		std::cout << "To: ";
		std::cin >> b;
		std::cout << "From: ";
		std::cin >> c;
		std::cout << "Row: ";
		std::cin >> d;
		game.doMove(game.generateMove(a, b, c, d));
	}
	if (game.gameWon()) 
		std::cout << "YOU WON!"<<endl;
	else
		std::cout << "Game Over" << endl;
}

int main()
{
	srand(time(0));
	std::cout << "Random Deals" << endl;
	AIPlay(4000, 0);
	std::cout << "\nWinable Deals" << endl;
	AIPlay(4000,1);
	//KlondikeGameController game = KlondikeGameController(1);
	////game.showScoreAndBoard();
	//BoardScoreOptimiser bso = BoardScoreOptimiser();
	//bso.grow();

}

