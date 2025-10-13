#pragma once
#include "KlondikeGameController.h"

class DummyPlayer : public KlondikeGameController
{

	struct ActionValue {
		int score;
		KlondikeGameController::Move move;

		ActionValue(int p, KlondikeGameController::Move m) {
			score = p;
			move = m;
		}
	};

	struct CompareTask {
		bool operator()(const ActionValue& t1, const ActionValue& t2) {
			return t1.score < t2.score;
		}
	};

	public: 
		DummyPlayer() {};

		void makeBestMove();
		void makeRandomMove();
		void suidRandomMove();

	private:

		GameBoard copyBoard() {
			GameBoard g = GameBoard(this->stockPile, this->drawnCards, this->columns, this->foundation);
			return g;
		}

		void doMove(KlondikeGameController::Move m, GameBoard* board);
};

