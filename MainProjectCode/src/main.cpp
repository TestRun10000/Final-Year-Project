

#include <iostream>
#include<fstream>
#include<random>
#include "DummyPlayer.h"
#include"MCTS.h"
#include<thread>
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


	int moves=0;
	winCount = 0;
	for (int i = 0; i < numGames; i++) {
		std::cout << i << "/" << numGames << " \r";
		std::cout.flush();
		DummyPlayer game(bs, dealType);
		while (!game.isGameOver()) {
			moves++;
			game.bestMoveRandom();
		}
		if (game.gameWon()) {
			winCount++;
		}
	}
	std::cout << "Playing the Best Moves most of the time resulsts in" << endl;
	std::cout << "Games Won: " << winCount << endl;
	std::cout << "This is a Win rate of:" << (winCount / (float)numGames) * 100 << "%" << endl;
	std::cout << "average number of moves: " << (moves / numGames) << endl;
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

void runSingleMCTS(int* win, int mode) {
	KlondikeGameController game(mode);
	//DummyPlayer game(mode);
	set<Card> seen = game.visableCards();
	int moves = 0;
	while (!game.isGameOver())
	{
		if (moves > 140) {
			break;
		}
		MCTS mcts(game.getBoard(), seen, game.getLastMove());
		game.doMove(mcts.getAction());
		//game.showScoreAndBoard();
		for (auto c : game.visableCards())
			seen.insert(c);
		moves++;
	}
	if (game.gameWon()) {
		//std::cout << "Winner!" << endl;
		*win = 1;
	}
	else {
		*win = 0;
	}
}

void MCTSPlay(int games,int mode) {
	int winCount = 0;
	std::ofstream file("Last Run.txt", std::ios_base::app);
	for (int i = 0; i < games;) {
		std::cout << i  << "/" << games << " \r";
		std::cout.flush();
		vector<thread> threads;
		std::vector<int> rewards(10 , 0);
		for (int j = 0; j < 10; j++) {
			if (i + j > games) {
				break;
			}
			int idx = j;
			threads.emplace_back([&rewards, j,mode]() {
				int result = 0;
				runSingleMCTS(&result, mode);
				rewards.at(j) = result;
				});

		}
		for (auto& t : threads)
		{
			t.join();
			i++;
		}
		for (int w : rewards) {
			winCount += w;
		}
		//game.showScoreAndBoard();

		
	}
	std::cout << "Games Won: " << winCount << endl;
	file << "Games Won: " << winCount << endl;
	std::cout << "This is a Win rate of:" << (winCount / (float)games) * 100 << "%" << endl << endl << endl;
	file << "This is a Win rate of:" << (winCount / (float)games) * 100 << "%" << endl<<endl<<endl;
	file.close();
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
	float cur=best;
	float curS = bestScore;
	file << best << "," << bestScore << "\n";
	fileb << 500 << "," << bestScore << "\n";
	for (int i = 1; i < 1001;i++) {
		std::cout << i << "/" << 1001 << " \r";
		std::cout.flush();
		float candidate =cur+shift(gen);
		if (candidate > 0.9) {
			candidate = 1  - candidate;
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
	std::cout << "       "<<endl;
	std::cout << best << " " << bestScore;
	filec << best << " " << bestScore;
	file.close();
	fileb.close();
	filec.close();
}

int main()
{

	//std::cout << "Random Deals" << endl;
	//AIPlay(2000, 0);
	//std::cout << "\nWinable Deals" << endl;
	//AIPlay(2000,1);
	//KlondikeGameController game = KlondikeGameController(1);
	////game.showScoreAndBoard();
	//BoardScoreOptimiser bso = BoardScoreOptimiser();
	//bso.grow();
	//int a = 0;
	//runSingleMCTS(&a,0);
	//std::cout << a << endl;
	MCTSPlay(100, 0);
	//MCTSPlay(100,1);
	//std::cout <<(long) std::thread::hardware_concurrency();
	//_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
	//_CrtDumpMemoryLeaks();

}

