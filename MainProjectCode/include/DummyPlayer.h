#pragma once
#include "KlondikeGameController.h"
#include"BoardScorer.h"
#include<random>
#include<cmath>

class DummyPlayer : public KlondikeGameController
{
protected:
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
		BoardScorer bso;
		std::mt19937 gen;

	public: 
		DummyPlayer() :KlondikeGameController(0) {
			this->bso = BoardScorer();
		}
		DummyPlayer(int i) :KlondikeGameController(i) {
			this->bso = BoardScorer();
		};

		DummyPlayer(float weights[5],int i=0) :KlondikeGameController(i) {
			this->bso = BoardScorer(weights);
		}

		DummyPlayer(BoardScorer b, int i = 0) :KlondikeGameController(i) {
			this->bso = b;
		}
	
		DummyPlayer(GameBoard b) :KlondikeGameController(b) {
			this->bso = BoardScorer();
		}

		GameBoard::Move makeBestMove();
		GameBoard::Move makeRandomMove();
		GameBoard::Move suidRandomMove();
		GameBoard::Move bestMoveRandom(float s = 0.359925);

		void doMove(GameBoard::Move m);

		

	protected:

		GameBoard copyBoard() {
			/*GameBoard g =;*/
			return GameBoard(*this->stockPile, *this->drawnCards, *this->columns, this->foundation);
		}


		void doMove(GameBoard::Move m, GameBoard* board);
};

