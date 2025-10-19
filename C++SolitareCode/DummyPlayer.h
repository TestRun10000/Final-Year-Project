#pragma once
#include "KlondikeGameController.h"
#include"BoardScorer.h"
class DummyPlayer : public KlondikeGameController
{

	struct ActionValue {
		float score;
		KlondikeGameController::Move move;

		ActionValue(float p, KlondikeGameController::Move m) {
			score = p;
			move = m;
		}
	};

	struct CompareTask {
		bool operator()(const ActionValue& t1, const ActionValue& t2) {
			return t1.score < t2.score;
		}
	};
	private:
		BoardScorer bso;

	public: 
		DummyPlayer() :KlondikeGameController(0) {
			this->bso = BoardScorer();
		}
		DummyPlayer(int i) :KlondikeGameController(i) {
			float s[5] = { 10.818811,1.636655,6.108121,0.444798,6.098790 };
			this->bso = BoardScorer(s);
		};

		DummyPlayer(float weights[5],int i=0) :KlondikeGameController(i) {
			this->bso = BoardScorer(weights);
		}

		DummyPlayer(BoardScorer b, int i = 0) :KlondikeGameController(i) {
			this->bso = b;
		}

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

