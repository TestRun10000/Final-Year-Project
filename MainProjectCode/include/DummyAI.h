#pragma once
#include"DummyPlayer.h"
#include"PPOHelpers.h"

class DummyAI : public DummyPlayer
{
private:
	NetDeep critic = NetDeep(30, 1, 512);
	bool criticLoaded = false;
public:

	DummyAI(BoardScorer bs, int i) :DummyPlayer(bs, i) {
		this->critic->to(torch::kCUDA);
	}

	GameBoard::Move makeBestMove();

	void loadCritic(string path) {
		try {
			torch::load(this->critic, path);
			criticLoaded = true;
		}
		catch (const c10::Error& e) {
			std::cout << path + " moded did not load.\n";
		}
	}
};