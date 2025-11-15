#pragma once
#include "KlondikeBoard.h"
#include <vector>
#include<iostream>
class KlondikeGameController : public GameBoard
{




private:
	int LastTopScore = 0;
	int LastNumberOfMove = 1;
	int movesSinceLastScoreChange = 0;
	int presNumberOfMoves=0;
	Move lastMove=Move();


public:
	KlondikeGameController(int i=0) {

		if (i == 0)
			randomDeal();
		else
			winableDeal();
	}

	KlondikeGameController(GameBoard b) :GameBoard(b) {}

	vector<Move> getAvailableMoves();

	void doMove(Move m);

	void showScoreAndBoard() {
		std::cout << this->toString()<<endl;
	};

	Move generateMove(int m, int toCol = 0, int fromCol = 0, int row = 0) {
		return GameBoard::generateMove(m, toCol, fromCol, row);
	}
	
	static std::string moveToString(Move m);
	
	bool gameWon();
	
	bool isGameOver();

	GameBoard getBoard() {
		return GameBoard(*this->stockPile, *this->drawnCards, *this->columns, this->foundation);
	}

	bool isReverseMove(Move m);

	void setLastMove(Move m) { this->lastMove = m; }

	Move getLastMove() { return this->lastMove; }
};


