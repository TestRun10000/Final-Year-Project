#include "KlondikeBoard.h"
#include<iostream>

void GameBoard::initGame() {
	this->stockPile->shuffle();
	for (int i = 0; i < 7; i++) {
		for (int j = i; j < 7; j++) {
			Card* c = this->stockPile->draw();
			if (j == i) {
				c->reveal();
			}
			this->columns->pushCardToColumn(j,c);
		}
	}
}

void GameBoard::drawCardsFromStockPile(unsigned int count) {
	if (this->stockPile->isEmpty()) {
		int reStockCount = this->drawnCards->numInDeck();
		for (unsigned int i = 0; i < reStockCount; i++)
		{
			if (this->drawnCards->isEmpty())
				break;
			Card* drawn = this->drawnCards->draw();
			drawn->hide();
			this->stockPile->push(drawn);
		}
		this->score -= 50;
	}
	for (unsigned int i = 0; i < count; i++)
	{
		if (this->stockPile->isEmpty())
			break;
		Card* drawn = this->stockPile->draw();
		drawn->reveal();
		this->drawnCards->push(drawn);
	}
	
}

void GameBoard::moveCardFromDrawn(unsigned int col) {
	Card* card = this->drawnCards->draw();
	if (card == nullptr) return;
	if (col >= 7) return;
	if (this->columns->isColEmpty(col)) {
		if (card->getRank() == 13) {
			this->columns->pushCardToColumn(col,card);
			this->score += 5;
		}
		return;
	}
	if (this->columns->getBottomCardFromColumn(col).doesStack(*card)) {
		this->columns->pushCardToColumn(col,card);
		this->score += 5;
	}
	else {
		this->drawnCards->push(card);
	}
}

void GameBoard::removeFromFoundation(unsigned int to, unsigned int from) {
	if (from >= 4 || to>=7) return;
	if (this->foundation[from].empty()) return;
	Card* card = this->foundation[from].top();
	if (this->columns->getBottomCardFromColumn(to).doesStack(*card)) {
		this->columns->pushCardToColumn(to,card);
		this->foundation[from].pop();
		this->score -= 10;
	}
}

void GameBoard::placeInFoundationFromCol(unsigned int col) {
	if (col >= 7) return;

	if (this->columns->isColEmpty(col)) return;
	Card c = this->columns->getBottomCardFromColumn(col);
	int foundationId = (int)c.getSuit();
	if (c.getRank() == 1) {
		this->foundation[foundationId].push(this->columns->popFromColumn(col));
		this->score += 10;
	}
	else if (this->foundation[foundationId].top()->doesStack(c, false, true)) {
		this->foundation[foundationId].push(this->columns->popFromColumn(col));
		this->score += 10;
	}
}

void GameBoard::placeInFoundationFromDraw() {
	Card* c = this->drawnCards->draw();
	int foundationId = (int)c->getSuit();
	if (c->getRank() == 1) {
		this->foundation[foundationId].push(c);
		this->score += 10;
	}
	else if (this->foundation[foundationId].top()->getRank() == c->getRank() - 1) {
		this->foundation[foundationId].push(c);
		this->score += 10;
	}
	else {
		this->drawnCards->push(c);
	}
}

bool GameBoard::moveCardsFromColtoCol(unsigned int fromCol, unsigned int fromRow,unsigned int toCol) {
	return this->columns->moveCardsFromColtoCol(fromCol, fromRow, toCol);

}

std::string GameBoard::toString() {
	string deckValue = this->stockPile->isEmpty() ? "[  ]" : "####";
	string drawPile = this->drawnCards->isEmpty()? "[  ]" : this->drawnCards->peek().toShortString();
	string top = " " + deckValue + " : " + drawPile;
	top += string(10,' ');
	for (int i = 0; i < 4; i++) { 
		top += !this->foundation[i].empty()? this->foundation[i].top()->toShortString():"[  ]";
		if (i < 3) {
			top += " : ";
		}
	}
	string colsStrings[7];
	string out = top + "\n "+string(46,'-')+"\n ";

	out = out+this->columns->toString() + "\n\n\n";
	//std::cout << top << endl;
	return out;
}

int GameBoard::boardScore() {
	int score = 0;
	//empty colums, and how many cards have been revealed 
	for (int i = 0; i < 7; i++) {
		if (this->columns->isColEmpty(i)) {
			score += 9;
		}
		else {
			if (this->columns->getFirstVisFromCol(i).getRank() == 13) {
				score += 10;
			}
			int numShown = i - this->columns->getPosOfFistVisOfCol(i);
			score += numShown * 10;
		}

	}
	//how many cards have been added to the foundation
	for (int i = 0; i < 4; i++) {
		if (!this->foundation[i].empty()) {
			if (this->foundation[i].top()->getRank() > 0) {
				score += this->foundation[i].top()->getRank() * 30;
			}
		}
	}
	//how many cards taken from the deck
	score += 8 * (24 - (this->stockPile->numInDeck() + this->drawnCards->numInDeck()));

	return score;
}

