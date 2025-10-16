#include "KlondikeBoard.h"
#include<iostream>

void GameBoard::randomDeal() {
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

void GameBoard::winableDeal() {

	this->createWinStateBoard();
	list<GameBoard::Move> moves = getReverseMoves();

	//if count exceeds x or end but not in final state start again
	do{
		int j = (rand() % moves.size());
		Move m = generateMove(0, 0, 0, 0);
		int x = 0;
		for (const auto& mov : moves) {
			if (x == j) {
				m = mov;
				break;
			}
			x++;
		}
		doReverseMove(m);
		moves = getReverseMoves();
	} while (!moves.empty());
	moves = getReverseMoves();

}
list<GameBoard::Move> GameBoard::getReverseMoves() {
	list<GameBoard::Move> moves = list<GameBoard::Move>();
	//from column
	for (int i = 0; i < 7; i++) {
		if (this->columns->isColEmpty(i) ||this->columns->getPosOfFistVisOfCol(i)==-1) {
			continue;
		}
		Card top = this->columns->getFirstVisFromCol(i);
		int topPos = this->columns->getPosOfFistVisOfCol(i);
		int numOfCardStacked = this->columns->getLengthOfCol(i) - topPos;
		if(topPos == 0 && this->columns->getLengthOfCol(i) - numOfCardStacked < i)
			moves.push_back(generateMove(MoveType::hideCard, 0, i, 0));
		else if (topPos > 0 && this->columns->getLengthOfCol(i)-numOfCardStacked<i) {
			moves.push_back(generateMove(MoveType::hideCard, 0, i, 0));
			return moves;
		}
		else if (numOfCardStacked + topPos == i + 1) continue;
		for (int j = 0; j < 7; j++) {
			if (j == i) continue;
			if (this->columns->isColEmpty(j) || this->columns->getPosOfFistVisOfCol(j) == -1 ) {
				for (int x = topPos; x < this->columns->getLengthOfCol(i); x++)
					moves.push_back(generateMove(MoveType::colToCol, j, i, x));
				continue;
			}
			Card bottom = this->columns->getBottomCardFromColumn(j);
			int diffInRank = top.getRank() - bottom.getRank();
			if (bottom.getColour() != top.getColour()) {
				if (diffInRank == -1 &&   !(topPos > 0))
					moves.push_back(generateMove(MoveType::colToCol, j, i, topPos));
				//if diff in rank +1 is less than numof card stack the stack contains a card that could be placed in that column
				else if (numOfCardStacked > diffInRank + 1 && diffInRank > 1 && diffInRank % 2 == 1) {
					moves.push_back(generateMove(MoveType::colToCol, j, i, topPos + diffInRank + 1));
				}
			}
			else if (numOfCardStacked > diffInRank + 1 && diffInRank > 0 && diffInRank % 2 == 0) {
				moves.push_back(generateMove(MoveType::colToCol, j, i, topPos + diffInRank + 1));
			}
		}
		Card bottom = this->columns->getBottomCardFromColumn(i);
		if (bottom.getRank() == 1 &&!this->allOpeningCardsDelt(true)) {
			moves.push_back(generateMove(MoveType::colToFoundation, 0, i));
		}
		else if (!this->foundation[bottom.getSuit()].empty() && this->foundation[bottom.getSuit()].top()->getRank() == bottom.getRank() - 1) {
			if(!this->allOpeningCardsDelt(true))
				moves.push_back(generateMove(MoveType::colToFoundation, 0, i));
		}
		if (this->stockPile->numInDeck() < 24 ) {
			moves.push_back(generateMove(MoveType::drawToCol, 0, i));
		}
	}
	//from foundation
	for (int i = 0; i < 4; i++) {
		if (this->foundation[i].empty()) {
			continue;
		}
		if (this->stockPile->numInDeck() < 24) {
			moves.push_back(generateMove(MoveType::drawToFoundation, 0, i));
		}
		Card card = *this->foundation[i].top();
		for (int j = 0; j < 7; j++) {
			if (this->columns->isColEmpty(j) || this->columns->getPosOfFistVisOfCol(j) == -1) {
				moves.push_back(generateMove(MoveType::foundationToCol, j, i));
				continue;
			};
			if (this->columns->getBottomCardFromColumn(j).doesStack(card)) {
				moves.push_back(generateMove(MoveType::foundationToCol, j, i));
			}
		}
	}
	return moves;
}

void  GameBoard::doReverseMove(Move m){
	switch (m.moveType)
	{
		case colToCol:
			this->columns->moveCardsFromColtoCol(m.fromCol, m.row, m.toCol,true);
			break;
		case colToFoundation:
			placeInFoundationFromCol(m.fromCol);
			break;
		case drawToFoundation:
			this->stockPile->push(this->foundation[m.fromCol].top());
			this->foundation[m.fromCol].pop();
			break;
		case foundationToCol:
			removeFromFoundation(m.toCol, m.fromCol,true);
			break;
		case drawToCol:
			this->stockPile->push(this->columns->popFromColumn(m.fromCol,true));
			break;
		case hideCard:
			this->columns->hideFirstVisOfCol(m.fromCol);
			break;
	}
}

void GameBoard::createWinStateBoard() {
	this->stockPile = new Deck();//replaces a full deck with a empty one 
	for (int i = 0; i < 4; i++) {
		Card* c = new Card((Card::Suits)i, 13);
		c->reveal();
		this->stockPile->push(c);
	}
	this->stockPile->shuffle();
	this->columns = new CardColumnCollection();
	for (int i = 0; i < 4; i++) 
		this->columns->pushCardToColumn(i, this->stockPile->draw());
	for (int i = 12; i > 0; i--) {
		bool suitUsed[4] = { false, false, false, false };
		for (int j = 0; j < 4; j++) {
			int colour = this->columns->getBottomCardFromColumn(j).getColour();
			int r = colour==1 ? (rand() % 2) + 2: (rand() % 2);
			if (!suitUsed[r]) {
				Card* c = new Card((Card::Suits)r, i);
				c->reveal();
				this->columns->pushCardToColumn(j, c);
				suitUsed[r] = true;
			}
			else {
				if (colour == 1) 
					r = r == 2 ? 3 : 2;
				else
					r = r == 0 ? 1 : 0;
				Card* c = new Card((Card::Suits)r, i);
				c->reveal();
				this->columns->pushCardToColumn(j, c);
				suitUsed[r] = true;
			}
		}
	}
	//for (int i = 0; i < 52; i++) {
	//	Card* c = this->stockPile->draw();
	//	c->reveal();
	//	this->foundation[c->getSuit()].push(c);
	//}
}

bool GameBoard::allOpeningCardsDelt(bool hiddenOnly) {
	for (int i = 0; i < 7; i++) {
		int lenOfCol = this->columns->getLengthOfCol(i);
		if (!(this->columns->getPosOfFistVisOfCol(i) == -1 && lenOfCol == i)&&
			!(this->columns->getPosOfFistVisOfCol(i) != -1 && lenOfCol - (lenOfCol - this->columns->getPosOfFistVisOfCol(i)) == i)&&hiddenOnly) 
		{
			return false;
		}
		else if(!hiddenOnly && this->columns->getLengthOfCol(i)!=i+1)
			return false;
	}
	return true;
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

void GameBoard::removeFromFoundation(unsigned int to, unsigned int from,bool allowAnyEmpty) {
	if (from >= 4 || to>=7) return;
	if (this->foundation[from].empty()) return;
	Card* card = this->foundation[from].top();
	if (this->columns->isColEmpty(to)&&!allowAnyEmpty) return;
	else if ((this->columns->isColEmpty(to)||this->columns->getPosOfFistVisOfCol(to)==-1) && allowAnyEmpty) {
		this->columns->pushCardToColumn(to, card);
		this->foundation[from].pop();
		return;
	}
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
		else if (this->columns->getPosOfFistVisOfCol(i) == -1) continue;
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

