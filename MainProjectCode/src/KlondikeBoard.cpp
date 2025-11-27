#include "KlondikeBoard.h"
#include<iostream>
#include<random>



#pragma region  Deal
void GameBoard::randomDeal() {
	this->stockPile->shuffle();
	for (int i = 0; i < 3; i++) {
		for (int j = i; j < 3; j++) {
			Card c = this->stockPile->draw();
			if (j == i) {
				c.reveal();
			}
			this->columns->pushCardToColumn(j,c);
		}
	}
}

void GameBoard::winableDeal() {

	this->createWinStartBoard();
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
	ShuffleTopCards();
}

void GameBoard::ShuffleTopCards() {
	this->stockPile->shuffle();
	int colPos[7] = { 0,1,2,3,4,5,6 };
	int temp = 0;
	for (int i = 6; i > 0; i--) {
		int j = (rand() % i) + 1;
		temp = colPos[i];
		colPos[i] = colPos[j];
		colPos[j] = temp;

	}
	for (int i = 0; i < 7; i++) {
		Card c = this->stockPile->draw();
		int col = colPos[i];
		Card x = this->columns->popFromColumn(col, true);
		this->columns->pushCardToColumn(col, c);
		this->drawnCards->push(x);
	}
	for (int i = 0; i < 7; i++)
		this->stockPile->push(this->drawnCards->draw());
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
		else if (!this->foundation[bottom.getSuit()].empty() && this->foundation[bottom.getSuit()].top().getRank() == bottom.getRank() - 1) {
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
		Card card = this->foundation[i].top();
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

void GameBoard::createWinStartBoard() {
	delete this->stockPile;
	this->stockPile = new Deck();//replaces a full deck with a empty one 
	for (int i = 0; i < 4; i++) {
		Card c =  Card((Card::Suits)i, 13);
		c.reveal();
		this->stockPile->push(c);
	}
	this->stockPile->shuffle();
	for (int i = 0; i < 4; i++) 
		this->columns->pushCardToColumn(i, this->stockPile->draw());
	for (int i = 12; i > 0; i--) {
		bool suitUsed[4] = { false, false, false, false };
		for (int j = 0; j < 4; j++) {
			int colour = this->columns->getBottomCardFromColumn(j).getColour();
			int r = colour==1 ? (rand() % 2) + 2: (rand() % 2);
			if (!suitUsed[r]) {
				Card c =  Card((Card::Suits)r, i);
				c.reveal();
				this->columns->pushCardToColumn(j, c);
				suitUsed[r] = true;
			}
			else {
				if (colour == 1) 
					r = r == 2 ? 3 : 2;
				else
					r = r == 0 ? 1 : 0;
				Card c =  Card((Card::Suits)r, i);
				c.reveal();
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

void GameBoard::partailDeal(set<Card> cardPool, const GameBoard& base) {
	delete this->stockPile;
	delete this->drawnCards;
	this->stockPile = new Deck();
	this->drawnCards =new Deck(*base.drawnCards);
	for (auto card : cardPool) {
		this->stockPile->push(card);
	}
	if (!this->stockPile->isEmpty()) {
		this->stockPile->shuffle();
	}
	for (int i = 0; i < 4; i++) {
		this->foundation[i] = base.foundation[i];
	}
	for (int i = 0; i < 7; i++) {
		for (int j = 0; j < base.columns->getLengthOfCol(i); j++) {
			if (j < base.columns->getPosOfFistVisOfCol(i)) {
				this->columns->pushCardToColumn(i,this->stockPile->draw());
			}
			else {
				this->columns->pushCardToColumn(i, base.columns->getCard(i, j));
			}
		}
	}
	if (this->stockPile->numInDeck() == 0) {
		delete this->stockPile;
		this->stockPile = new Deck(*base.stockPile);
	}
}

#pragma endregion

#pragma region CardMovement
void GameBoard::drawCardsFromStockPile(unsigned int count) {
	if (this->stockPile->isEmpty()) {
		int reStockCount = this->drawnCards->numInDeck();
		for (unsigned int i = 0; i < reStockCount; i++)
		{
			if (this->drawnCards->isEmpty())
				break;
			Card drawn = this->drawnCards->draw();
			drawn.hide();
			this->stockPile->push(drawn);
		}
		this->score -= 50;
	}
	for (unsigned int i = 0; i < count; i++)
	{
		if (this->stockPile->isEmpty())
			break;
		Card drawn = this->stockPile->draw();
		drawn.reveal();
		this->drawnCards->push(drawn);
	}
	
}

void GameBoard::moveCardFromDrawn(unsigned int col) {
	Card card = this->drawnCards->draw();
	card.reveal();
	if (card.isBlank()) return;
	if (col >= 7) return;
	if (this->columns->isColEmpty(col)) {
		if (card.getRank() == 13) {
			this->columns->pushCardToColumn(col,card);
			this->score += 5;
		}
		return;
	}
	if (this->columns->getBottomCardFromColumn(col).doesStack(card)) {
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
	Card card = this->foundation[from].top();
	if (this->columns->isColEmpty(to)&&!allowAnyEmpty) return;
	else if ((this->columns->isColEmpty(to)||this->columns->getPosOfFistVisOfCol(to)==-1) && allowAnyEmpty) {
		this->columns->pushCardToColumn(to, card);
		this->foundation[from].pop();
		return;
	}
	if (this->columns->getBottomCardFromColumn(to).doesStack(card)) {
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
	else if (this->foundation[foundationId].top().doesStack(c, false, true)) {
		this->foundation[foundationId].push(this->columns->popFromColumn(col));
		this->score += 10;
	}
}

void GameBoard::placeInFoundationFromDraw() {
	Card c = this->drawnCards->draw();
	int foundationId = (int)c.getSuit();
	if (c.getRank() == 1) {
		this->foundation[foundationId].push(c);
		this->score += 10;
	}
	else if (this->foundation[foundationId].top().getRank() == c.getRank() - 1) {
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
#pragma endregion

std::string GameBoard::toString() {
	string deckValue = this->stockPile->isEmpty() ? "[  ]" : "####";
	string drawPile = this->drawnCards->isEmpty()? "[  ]" : this->drawnCards->peek().toShortString();
	string top = " " + deckValue + " : " + drawPile;
	top += string(10,' ');
	for (int i = 0; i < 4; i++) { 
		top += !this->foundation[i].empty()? this->foundation[i].top().toShortString():"[  ]";
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
	float score = 0;
	score += 10.818811 * numOfCardsInFoundation();
	score += 1.636655f * numOfCardsRevealed();
	score += 6.108121f * (24 - numCardInDeck());
	score += 0.444798f * numOfEmptyCols();
	score += 6.098790f * numOfColsStartingWithKing();
	return score;
}

set<Card> GameBoard::visableCards() {
	set<Card> seen = set<Card>();
	for (int i = 0; i < 4; i++) {
		if (!this->foundation[i].empty()) {
			for (int j = 1; j <= this->foundation[i].top().getRank(); j++)
				seen.insert(Card((Card::Suits)i, j));
		}
	}
	if (!this->drawnCards->isEmpty())
		seen.insert(this->drawnCards->peek());
	for (int i = 0; i < 3; i++) {
		if (this->columns->isColEmpty(i)) continue;
		for (int j = this->columns->getPosOfFistVisOfCol(i); j < this->columns->getLengthOfCol(i); j++)
			seen.insert(this->columns->getCard(i, j));
	}
	return seen;
}


int GameBoard::EncodeMove(Move m) {
	switch (m.moveType)
	{
	case draw:
		return 0;
	case drawToFoundation:
		return 1;
	case foundationToCol:
		return 2 + m.fromCol * 3 + m.toCol;
	case colToFoundation:
		return 8 + m.fromCol + 3 * m.toCol;
	case drawToCol:
		return 14 + m.toCol;
	case colToCol:
		return 17 + (m.fromCol * 2 + (m.toCol < m.fromCol ? m.toCol : m.toCol - 1)) * 20 + m.row;
	default:
		return -1;
	}

}

GameBoard::Move GameBoard::DecodeMove(int m) {
	if (m == 0) return GameBoard::generateMove(0);
	else if (m == 1) return GameBoard::generateMove(1);
	else if (m >= 2 && m < 8) return GameBoard::generateMove(2, (m - 2) % 3, (m - 2) / 3);
	else if (m >= 8 && m < 14) return GameBoard::generateMove(3, (m - 8) / 3, (m - 8) % 3);
	else if (m >= 14 && m < 17) return GameBoard::generateMove(4, (m - 14));
	else if (m >= 17) {
		int base = ((m - 17) / 20);
		int from = base / 2;
		int to = base % 2 < from ? base % 2 : (base % 2) + 1;
		//int to = base<(base*7)/6? base/6:(base/6)+1;
		return GameBoard::generateMove(5, to, from, (m - 17) % 20);
	}

}



std::vector<int> GameBoard::EncodeBoard() {
	vector<int> board = vector<int>();

	board.push_back(!this->stockPile->isEmpty());
	std::vector<int>val = OneHotEncodeing(this->drawnCards->isEmpty() ? Card() : this->drawnCards->peek());
	board.insert(board.end(), val.begin(), val.end());
	for (int i = 0; i < 2; i++) {
		if (this->foundation[i].empty()) {
			board.push_back(0);
		}
		else {
			board.push_back(this->foundation[i].top().getRank());
		}
	}
	for (int i = 0; i < 3; i++) {
		std::vector<int>val = OneHotEncodeing(this->columns->isColEmpty(i) ? Card() : this->columns->getFirstVisFromCol(i));
		board.insert(board.end(), val.begin(), val.end());
		int first = this->columns->getPosOfFistVisOfCol(i);
		int length = this->columns->getLengthOfCol(i);
		board.push_back(length - first-1);
		board.push_back(first);
		val = OneHotEncodeing(this->columns->isColEmpty(i) ? Card() : this->columns->getBottomCardFromColumn(i));
		board.insert(board.end(), val.begin(), val.end());
	}
	return board;
}

std::vector<int> GameBoard::OneHotEncodeing(Card c) {
	std::vector<int> encoding = std::vector<int>(3);
	if (c.getRank() == -1) {
		return encoding;
	}
	encoding.at(0) = c.getRank();
	encoding.at(1+c.getSuit()) = 1;
	return encoding;
}