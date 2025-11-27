#include<KlondikeGameController.h>

vector<KlondikeGameController::Move> KlondikeGameController::getAvailableMoves()
{
	vector<KlondikeGameController::Move> possibleMoves = vector<KlondikeGameController::Move>();
	if (!this->drawnCards->isEmpty() || !this->stockPile->isEmpty()) {
		possibleMoves.push_back(generateMove(MoveType::draw));
	}
	//from drawn
	if (!this->drawnCards->isEmpty()) {
		Card drawnCard = this->drawnCards->peek();
		//to foundation
		if (drawnCard.getRank() == 1) {
			possibleMoves.push_back(generateMove(MoveType::drawToFoundation));
		}
		else {

			if (!this->foundation[drawnCard.getSuit()].empty() && this->foundation[drawnCard.getSuit()].top().getRank()==drawnCard.getRank()-1) {
				possibleMoves.push_back(generateMove(MoveType::drawToFoundation));
			}
			
		}
		//to column
		for (int i = 0; i < 3; i++) {
			if (this->columns->isColEmpty(i)){
				if(drawnCard.getRank() == 10) {
					possibleMoves.push_back(generateMove(MoveType::drawToCol, i)); 
				}
				continue;
			}
			else if (this->columns->getBottomCardFromColumn(i).doesStack(drawnCard)) {
				possibleMoves.push_back(generateMove(MoveType::drawToCol, i));
			}
		}

	}
	//from column
	for (int i = 0; i < 3; i++) {
		if (this->columns->isColEmpty(i)) {
			continue;
		}
		Card top = this->columns->getFirstVisFromCol(i);
		int topPos = this->columns->getPosOfFistVisOfCol(i);
		int numOfCardStacked = this->columns->getLengthOfCol(i)-topPos;
		for (int j = 0; j < 3; j++) {
			if (j == i) continue;
			//if (this->columns->getPosOfFistVisOfCol(j) != -1)continue;
			if (this->columns->isColEmpty(j)) {
				if(top.getRank() == 10 && topPos!=-1) {
					if(!isReverseMove(generateMove(MoveType::colToCol, j, i, topPos)))
						possibleMoves.push_back(generateMove(MoveType::colToCol, j, i, topPos));
				}
				continue;
			}
			
			Card bottom = this->columns->getBottomCardFromColumn(j);
			int diffInRank =top.getRank()-bottom.getRank();
			if (bottom.getColour() != top.getColour()) {
				if (diffInRank == -1) {
					if (!isReverseMove(generateMove(MoveType::colToCol, j, i, topPos)))
						possibleMoves.push_back(generateMove(MoveType::colToCol, j, i, topPos));
				}
				//if diff in rank +1 is less than numof card stack the stack contains a card that could be placed in that column
				else if (numOfCardStacked > diffInRank+1 && diffInRank > 1 && diffInRank%2==1) {
					if (!isReverseMove(generateMove(MoveType::colToCol, j, i, topPos+diffInRank+1)))
						possibleMoves.push_back(generateMove(MoveType::colToCol, j, i, topPos+diffInRank+1));
				}
			}
			else if (numOfCardStacked > diffInRank+1 && diffInRank>0 && diffInRank % 2 == 0) {
				if (!isReverseMove(generateMove(MoveType::colToCol, j, i, topPos+diffInRank+1)))
					possibleMoves.push_back(generateMove(MoveType::colToCol, j, i, topPos + diffInRank+1));
			}
		}
		Card bottom = this->columns->getBottomCardFromColumn(i);
		if (bottom.getRank() == 1) {
			if (!isReverseMove(generateMove(MoveType::colToFoundation, bottom.getSuit(), i)))
			possibleMoves.push_back(generateMove(MoveType::colToFoundation, bottom.getSuit(), i));
		}
		else if (!this->foundation[bottom.getSuit()].empty() && this->foundation[bottom.getSuit()].top().getRank() == bottom.getRank() - 1) {
			if(!isReverseMove(generateMove(MoveType::colToFoundation, bottom.getSuit(), i)))
				possibleMoves.push_back(generateMove(MoveType::colToFoundation, bottom.getSuit(),i));
		}
	}
	//from foundation
	for (int i = 0; i < 4; i++) {
		if (this->foundation[i].empty()) {
			continue;
		}
		Card card = this->foundation[i].top();
		for (int j = 0; j < 3; j++) {
			if (this->columns->isColEmpty(j)||this->columns->getPosOfFistVisOfCol(j) == -1) continue;
			if (this->columns->getBottomCardFromColumn(j).doesStack(card)) {
				if(!isReverseMove(generateMove(MoveType::foundationToCol, j, i)))
					possibleMoves.push_back(generateMove(MoveType::foundationToCol, j, i));
			}
		}
	}
	return possibleMoves;
}

void KlondikeGameController::doMove(KlondikeGameController::Move m)
{
	this->lastMove = m;
	if (m.toCol == 0 && m.moveType==drawToCol)
		int i = 0;
	switch (m.moveType)
	{
	case draw:
		this->drawCardsFromStockPile(1);
		break;
	case colToCol:
		if (!this->moveCardsFromColtoCol(m.fromCol, m.row, m.toCol)) {
			getAvailableMoves();
		}
		break;
	case colToFoundation:
		this->placeInFoundationFromCol(m.fromCol);
		break;
	case drawToFoundation:
		this->placeInFoundationFromDraw();
		break;
	case foundationToCol:
		this->removeFromFoundation(m.toCol, m.fromCol);
		break;
	case drawToCol:
		this->moveCardFromDrawn(m.toCol);
		break;
	}
	//if (gameWon()) {
	//	std::cout << "You Have Won";
	//}
	//this->showScoreAndBoard();
}

std::string KlondikeGameController::moveToString(Move m) {
	switch (m.moveType)
	{
	case draw:
		return "Move:draw";
	case colToCol:
		return "Move from column:" + std::to_string(m.fromCol) + " and row:" + std::to_string(m.row) + " To:" + std::to_string(m.toCol);
	case colToFoundation:
		return "Move from column:" + std::to_string(m.fromCol) + " and row:" + std::to_string(m.row) + " To foundation";
		break;
	case drawToFoundation:
		return "Move from draw to foundation";
	case foundationToCol:
		return "Move from foundation To:" + std::to_string(m.toCol);
	case drawToCol:
		return "Move from draw To column:" + std::to_string(m.toCol);
	default:
		return "Null";
	}

}

bool KlondikeGameController::gameWon() {
	for (int i = 0; i < 3; i++) {
		if (this->columns->getPosOfFistVisOfCol(i) > 0) return false;
	}
	this->score += 1000;
	return true;
}

bool KlondikeGameController::isGameOver() {
	if (gameWon()) return true;
	int currentScore = this->boardScore();
	if (this->getAvailableMoves().size() == 0) return true;
	if (currentScore <= this->LastTopScore) {
		this->movesSinceLastScoreChange++;
	}
	else {
		this->LastTopScore = currentScore;
		this->movesSinceLastScoreChange = 0;
	}
	if (this->movesSinceLastScoreChange >= 28) {
		//std::cout << "GameOver";
		return true;
	}
	return false;
}

bool KlondikeGameController::isReverseMove(Move m) {
	if (m.moveType == MoveType::colToCol || m.moveType == MoveType::foundationToCol || m.moveType == MoveType::colToFoundation) {
		return m.fromCol == this->lastMove.toCol && m.toCol == this->lastMove.fromCol;
	}
	return false;
}