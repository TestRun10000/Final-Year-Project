

#include <iostream>
#include<fstream>
#include<random>
#include "DummyPlayer.h"
#include"DummyAI.h"
#include "PPO.h"


//#define _CRTDBG_MAP_ALLOC
//#include <stdlib.h>
//#include <crtdbg.h>
//#ifdef _DEBUG
//#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
//// Replace _NORMAL_BLOCK with _CLIENT_BLOCK if you want the
//// allocations to be of _CLIENT_BLOCK type
//#else
//#define DBG_NEW new
//#endif
//note to self boardScore optimsation needs re doing as it is should of impacted end state. End game scoreing was left as static and did not update with weights 
//this does not seem to have impacted training but would be better for consistancey 
static void AIPlay(int numGames, int dealType) {

	int winCount = 0;
	float s[5] = { 10.818811,1.636655,6.108121,0.444798,6.098790 };
	BoardScorer bs = BoardScorer(s);
	for (int i = 0; i < numGames; i++) {
		DummyPlayer game(bs, dealType);
		//game = *new DummyPlayer(bs,dealType);
		std::cout << i << "/" << numGames << " \r";
		std::cout.flush();
		while (!game.isGameOver()) {
			game.doMove(game.makeBestMove());

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
		DummyPlayer game(bs, dealType);
		//game = DummyPlayer(dealType);
		while (!game.isGameOver()) {
			game.doMove(game.makeRandomMove());
		}
		if (game.gameWon()) {
			winCount++;
		}
	}
	std::cout << "Playing the Random Moves resulsts in" << endl;
	std::cout << "Games Won: " << winCount << endl;
	std::cout << "This is a Win rate of:" << (winCount / (float)numGames) * 100 << "%" << endl << endl;

	int moves = 0;
	winCount = 0;
	for (int i = 0; i < numGames; i++) {
		std::cout << i << "/" << numGames << " \r";
		std::cout.flush();
		DummyPlayer game(bs, dealType);
		while (!game.isGameOver()) {
			moves++;
			game.doMove(game.bestMoveRandom());
		}
		if (game.gameWon()) {
			winCount++;
		}
	}
	std::cout << "Playing the Best Moves most of the time resulsts in" << endl;
	std::cout << "Games Won: " << winCount << endl;
	std::cout << "This is a Win rate of:" << (winCount / (float)numGames) * 100 << "%" << endl;
	std::cout << "average number of moves: " << (moves / numGames) << endl << endl;
	//moves = 0;
	//winCount = 0;
	//for (int i = 0; i < numGames; i++) {
	//	std::cout << i << "/" << numGames << " \r";
	//	std::cout.flush();
	//	DummyAI game(bs, dealType);
	//	game.loadCritic("C://Users//jonat//source//25-26_CE301_sale_jonathan//MainProjectCode//Resources//critic.pth");
	//	while (!game.isGameOver()) {
	//		moves++;
	//		game.doMove(game.makeBestMove());
	//	}
	//	if (game.gameWon()) {
	//		winCount++;
	//	}
	//}
	//std::cout << "Playing the Best Moves using the critic results in" << endl;
	//std::cout << "Games Won: " << winCount << endl;
	//std::cout << "This is a Win rate of:" << (winCount / (float)numGames) * 100 << "%" << endl;
	//std::cout << "average number of moves: " << (moves / numGames) << endl << endl;
}


static void player() {
	KlondikeGameController game = KlondikeGameController();
	while (game.isGameOver()) {
		for (const auto& i : game.getAvailableMoves()) {
			std::cout << game.moveToString(i) << endl;
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
		std::cout << "YOU WON!" << endl;
	else
		std::cout << "Game Over" << endl;
}





int temp(float c) {
	srand(1);
	float s[5] = { 10.818811,1.636655,6.108121,0.444798,6.098790 };
	BoardScorer bs = BoardScorer(s);
	int winCount = 0;
	for (int i = 0; i < 100; i++) {
		DummyPlayer game(bs, 0);
		while (!game.isGameOver()) {
			game.bestMoveRandom(c);
		}
		if (game.gameWon()) {
			winCount++;
		}
	}
	return winCount;
}


void aneal() {
	std::ofstream file((std::string)RESOURCES_PATH + "aneal.csv");
	std::ofstream fileb((std::string)RESOURCES_PATH + "anealBest.csv");
	std::ofstream filec((std::string)RESOURCES_PATH + "BestAneal.txt");

	std::uniform_real_distribution<> chance(0, 1);
	std::uniform_real_distribution<> shift(-0.05, 0.05);
	std::uniform_real_distribution<> start(0, 0.9);
	std::mt19937 gen;
	float best = start(gen);
	int bestScore = temp(best);
	float cur = best;
	float curS = bestScore;
	file << best << "," << bestScore << "\n";
	fileb << 500 << "," << bestScore << "\n";
	for (int i = 1; i < 1001; i++) {
		std::cout << i << "/" << 1001 << " \r";
		std::cout.flush();
		float candidate = cur + shift(gen);
		if (candidate > 0.9) {
			candidate = 1 - candidate;
		}
		if (candidate < 0) {
			candidate = -candidate;
		}
		int canScore = temp(candidate);
		if (canScore > bestScore) {
			best = candidate;
			bestScore = canScore;
		}
		file << cur << "," << curS << "\n";
		float except = std::exp(-(canScore - curS) / (500.0 / (float)i));
		fileb << (500.0 / (float)i) << "," << bestScore << "\n";
		if (canScore - curS > 0 || chance(gen) < except) {
			cur = candidate;
			curS = canScore;
		}
	}
	std::cout << "       " << endl;
	std::cout << best << " " << bestScore;
	filec << best << " " << bestScore;
	file.close();
	fileb.close();
	filec.close();
}

int main()
{
	PPO ppo = PPO();
	ppo.learn();
	// 
	//KlondikeGameController g(0);
	//std::cout<<g.EncodeBoard().size()<<endl;
	//for (int i = 0; i < 136; i++) {
	//	std::cout << i<<" : " << KlondikeGameController::moveToString(GameBoard::DecodeMove(i)) << endl;
	//}
	srand(time(0));
	//ppo.Test();
	float avg = 0.0;
	for (int i = 0; i < 10; i++) {
		std::cout << "AI" << endl;
		avg+=ppo.Test();
		std::cout << "Random Deals" << endl;
		AIPlay(1000, 0);
		std::cout << "-------------------------------" << std::endl<<endl;
	}
	std::cout << avg / 10<<endl;
	////std::cout << "\nWinable Deals" << endl;
	//AIPlay(2000,1);

	std::cout << "Done";
	//BoardScoreOptimiser bso = BoardScoreOptimiser();
	//bso.grow();

	//std::cout <<(long) std::thread::hardware_concurrency();
	//_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
	//_CrtDumpMemoryLeaks();

}

