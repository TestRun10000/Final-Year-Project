#pragma once
#include "KlondikeBoard.h"
#include <list>
#include<iostream>
class KlondikeGameController : protected GameBoard
{


protected:

	enum MoveType
	{
		draw,
		colToCol,
		colToFoundation,
		drawToFoundation,
		foundationToCol,
		drawToCol
	};

	typedef struct Move{
		MoveType moveType;
		unsigned int fromCol,toCol,row;
	}Move;

private:
	int LastTopScore = 0;
	int LastNumberOfMove = 1;
	int movesSinceLastScoreChange = 0;
	int presNumberOfMoves;
	Move lastMove;


public:
	KlondikeGameController() { 
	initGame(); }

	list<Move> getAvailableMoves();

	void doMove(Move m);

	void showScoreAndBoard() {
		std::cout << "Score:" << this->boardScore() << endl;
		std::cout << this->toString()<<endl;
	};

	Move generateMove(int m,int toCol=0,int fromCol=0,int row=0) {
		Move newMove = Move();
		newMove.moveType = (MoveType)m;
		newMove.fromCol = fromCol;
		newMove.toCol = toCol;
		newMove.row = row;
		return newMove;
	}
	
	 std::string moveToString(Move m);

	 bool gameWon();

	 bool isGameOver();
};


