#pragma once
#include <stack>
#include <list>
#include <set>
#include "Deck.h"
#include "CardColumnCollection.h"
#include "Card.h"
#include <vector>
#include<bitset>
#include<array>

class GameBoard
{
protected:
	stack<Card> foundation[4];
	Deck* stockPile;
	Deck* drawnCards;
	CardColumnCollection* columns;
	int score = 0;
	array<bool,52> seenCards;

	public:
		enum MoveType
		{
			draw,
			drawToFoundation,
			foundationToCol,
			colToFoundation,
			drawToCol,
			colToCol,
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
	
		GameBoard(Deck stockpile, Deck drawnCards, CardColumnCollection tabula, stack<Card> foundation[4]) {
			this->stockPile = new Deck(stockpile);
			this->drawnCards = new  Deck(drawnCards);
			this->columns = new CardColumnCollection(tabula);
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

		
		~GameBoard() {
			delete this->stockPile;
			delete this->drawnCards;
			delete this->columns;
			//delete this->seenCards;
		}
	
		void randomDeal();

		void winableDeal();

		void partailDeal(set<Card> cardPool, const GameBoard& base);

		set<Card> visableCards();

#pragma region CardMovement
		void drawCardsFromStockPile(unsigned int count);
	
		void placeInFoundationFromCol(unsigned int col);
	
		void placeInFoundationFromDraw();
	
		void moveCardFromDrawn(unsigned int col);
	
		bool moveCardsFromColtoCol(unsigned int fromCol, unsigned int fromRow, unsigned int toCol);
	
		void removeFromFoundation(unsigned int to, unsigned int from, bool allowAnyEmpty=false);
	
		string toString();
	
		int getScore() { return this->score; }
	
		int boardScore();
#pragma endregion

#pragma region forScoring


		int numOfCardsInFoundation() {
			int n = 0;
			for (int i = 0; i < 4; i++) {
				n += this->foundation[i].size();
			}
			return n;
		}

		int numCardInDeck() {
			return this->stockPile->numInDeck() + this->drawnCards->numInDeck();
		}

		int numOfCardsRevealed() {
			int numShown = 0;
			for (int i = 0; i < 7; i++) {
				if (this->columns->getPosOfFistVisOfCol(i) == -1) continue;
				 numShown += i - this->columns->getPosOfFistVisOfCol(i);
			}
			return numShown;
		}

		int numOfEmptyCols() {
			int n = 0;
			for (int i = 0; i < 7; i++) 
				if (this->columns->isColEmpty(i))
					n++;
			return n;
		}

		int numOfColsStartingWithKing() {
			int n = 0;
			for (int i = 0; i < 7; i++) {
				if (this->columns->isColEmpty(i)) continue;
				else if(this->columns->getFirstVisFromCol(i).getRank() == 13 && this->columns->getPosOfFistVisOfCol(i) == 0)
					n++;
			}
			return n;
		}

#pragma endregion
	
		
		GameBoard(const GameBoard& base) {
			this->stockPile = new Deck(*base.stockPile);
			this->drawnCards = new Deck(*base.drawnCards);
			this->columns = new CardColumnCollection(*base.columns);
			for (int i = 0; i < 4; ++i) {
				this->foundation[i] = base.foundation[i];
			}
		}

		
		GameBoard& operator=(const GameBoard& base) {
			if (this != &base) {
				delete stockPile;
				delete drawnCards;
				delete columns;
				this->stockPile = new Deck(*base.stockPile);
				this->drawnCards = new Deck(*base.drawnCards);
				this->columns = new CardColumnCollection(*base.columns);
				for (int i = 0; i < 4; ++i) {
					this->foundation[i] = base.foundation[i];
				}
			}
			return *this;
		}

	
		GameBoard(GameBoard&& base) noexcept {
			this->stockPile = base.stockPile;
			this->drawnCards = base.drawnCards;
			this->columns = base.columns;
			for (int i = 0; i < 4; ++i) {
				this->foundation[i] = std::move(base.foundation[i]);
			}

			base.stockPile = nullptr;
			base.drawnCards = nullptr;
			base.columns = nullptr;
		}

		
		GameBoard& operator=(GameBoard&& base) noexcept {
			if (this != &base) {
				delete stockPile;
				delete drawnCards;
				delete columns;

				this->stockPile = base.stockPile;
				this->drawnCards = base.drawnCards;
				this->columns = base.columns;
				for (int i = 0; i < 4; ++i) {
					this->foundation[i] = std::move(base.foundation[i]);
				}

				base.stockPile = nullptr;
				base.drawnCards = nullptr;
				base.columns = nullptr;
			}
			return *this;
		}

		static int EncodeMove(Move m);
		static Move DecodeMove(int m);

		std::vector<int> EncodeBoard();

private:
		void createWinStartBoard();

		list<Move> getReverseMoves(); 

		void doReverseMove(Move m);

		bool allOpeningCardsDelt(bool hiddenOnly);

		void ShuffleTopCards();

		vector<int> OneHotEncodeing(Card c);
};