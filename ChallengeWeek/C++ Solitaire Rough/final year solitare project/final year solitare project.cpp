// final year solitare project.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "KlondikeGameController.h"
#include "DummyPlayer.h"


int main()
{
	//game.showScoreAndBoard();
	int winCount=0;
	int numGames = 4000;
	srand(time(0));
	for (int i = 0; i < numGames; i++) {
		DummyPlayer game;
		game = *new DummyPlayer();
		std::cout << i << "/" << numGames << " \r";
		std::cout.flush();
		while (!game.isGameOver()) {
			game.makeBestMove();
			//for (const auto& i : game.getAvailableMoves()) {
			//	std::cout << game.moveToString(i)<<endl;
			//}
			//int a, b, c, d;
			//std::cout << "Give action 0-Draw,1-colToCol,2-colToFoun,3-DrawToFoun,4-FounToCol,5-DrawToCol" << endl;
			//std::cin >> a;
			//std::cout << "To: ";
			//std::cin >> b;
			//std::cout << "From: ";
			//std::cin >> c;
			//std::cout << "Row: ";
			//std::cin >> d;
			//game.doMove(game.generateMove(a, b, c, d));

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
		DummyPlayer game;
		game = DummyPlayer();
		while (!game.isGameOver()) {
			game.makeRandomMove();
		
		}
		if (game.gameWon()) {
			winCount++;
		}
	}
	std::cout << "Playing the Random Moves resulsts in" << endl;
	std::cout << "Games Won: " << winCount<<endl;
	std::cout << "This is a Win rate of:" << (winCount / (float)numGames) * 100 << "%" << endl<<endl;

	winCount = 0;
	for (int i = 0; i < numGames; i++) {
		std::cout << i << "/" << numGames << " \r";
		std::cout.flush();
		DummyPlayer game;
		game = DummyPlayer();
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

