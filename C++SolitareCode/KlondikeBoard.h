#pragma once
#include <stack>
#include <list>
#include "Deck.h"
#include "CardColumnCollection.h"
#include "Card.h"

class GameBoard
{
	protected:
		stack<Card*> foundation[4];
		Deck* stockPile;
		Deck* drawnCards;
		CardColumnCollection* columns;
		int score = 0;

	protected:
		enum MoveType
		{
			draw,
			colToCol,
			colToFoundation,
			drawToFoundation,
			foundationToCol,
			drawToCol,
			hideCard
		};

		typedef struct Move {
			MoveType moveType;
			unsigned int fromCol, toCol, row;
		}Move;
	
	
	public:
		GameBoard() {
			this->stockPile = new Deck(4);
			this->drawnCards = new Deck();
			this->columns = new CardColumnCollection();
		}
	
		GameBoard(Deck* stockpile, Deck* drawnCards, CardColumnCollection* tabula, stack<Card*> foundation[4]) {
			this->stockPile = new Deck(*stockpile);
			this->drawnCards = new  Deck(*drawnCards);
			this->columns = new CardColumnCollection(*tabula);
			for (int i = 0; i < 4; i++) {
				this->foundation[i] = foundation[i];
			}
		}

		static Move generateMove(int m, int toCol = 0, int fromCol = 0, int row = 0) {
			Move newMove = Move();
			newMove.moveType = (MoveType)m;
			newMove.fromCol = fromCol;
			newMove.toCol = toCol;
			newMove.row = row;
			return newMove;
		}


	//does not work
		//~GameBoard() {
		//	delete stockPile;
		//	delete drawnCards;
		//	delete columns;
		//	delete[] foundation;
		//}
	
		void randomDeal();

		void winableDeal();
	
		void drawCardsFromStockPile(unsigned int count);
	
		void placeInFoundationFromCol(unsigned int col);
	
		void placeInFoundationFromDraw();
	
		void moveCardFromDrawn(unsigned int col);
	
		bool moveCardsFromColtoCol(unsigned int fromCol, unsigned int fromRow, unsigned int toCol);
	
		void removeFromFoundation(unsigned int to, unsigned int from, bool allowAnyEmpty=false);
	
		string toString();
	
		int getScore() { return this->score; }
	
		int boardScore();

	private:
		void createWinStateBoard();

		list<Move> getReverseMoves(); 

		void doReverseMove(Move m);

		bool allOpeningCardsDelt(bool hiddenOnly);
};