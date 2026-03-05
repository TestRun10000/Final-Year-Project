#include "DummyPlayer.h"
#include <queue>
#include<vector>



GameBoard::Move DummyPlayer::makeBestMove() {
	vector<KlondikeGameController::Move> moves = getAvailableMoves();
	std::priority_queue<ActionValue, std::vector<ActionValue>, CompareTask> task_queue;
	for (KlondikeGameController::Move m : moves) {
		GameBoard copy = DummyPlayer::copyBoard();
		DummyPlayer::doMove(m, &copy);
		int score = this->bso.boardScore(copy);
		task_queue.emplace(score, m);
	}
	/*KlondikeGameController::doMove(task_queue.top().move);*/
	return task_queue.top().move;
}

GameBoard::Move DummyPlayer::makeRandomMove() {
	vector<KlondikeGameController::Move> moves = getAvailableMoves();
	int j = (rand() % moves.size());
	KlondikeGameController::Move m = generateMove(0, 0, 0, 0);
	return moves.at(j);
}

GameBoard::Move DummyPlayer::bestMoveRandom(float s) {
	std::uniform_real_distribution<> chance(0, 1);

	float ran = chance(gen);
	if (ran <= s) {
		return makeRandomMove();
	}
	else {
		return makeBestMove();
	}
}


GameBoard::Move DummyPlayer::suidRandomMove() {
	vector<KlondikeGameController::Move> moves = getAvailableMoves();
	std::priority_queue<ActionValue, std::vector<ActionValue>, CompareTask> task_queue;
	float total = 0;
	for (KlondikeGameController::Move m : moves) {
		GameBoard copy = DummyPlayer::copyBoard();
		DummyPlayer::doMove(m, &copy);
		int score = this->bso.boardScore(copy);
		task_queue.emplace(score, m);
		total += score;
	}
	float ran = (rand() % 76);//represents 75% or if that is how mouch stronger the best move is 
	ActionValue lastPair = task_queue.top();
	task_queue.pop();
	if (ran>(lastPair.score * 100) / total ){
		//KlondikeGameController::doMove(lastPair.move);
		return lastPair.move;
	}
	for (int i = 1; i < task_queue.size()+1; i++) {
		ActionValue cur = task_queue.top();
		task_queue.pop();
		if (ran > (cur.score * 100) / total && ran <= (lastPair.score * 100) /total) {
			//KlondikeGameController::doMove(cur.move);
			return cur.move;
		}
		lastPair = cur;
	}
	return lastPair.move;
}



void DummyPlayer::doMove(GameBoard::Move m) {
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
}

void DummyPlayer ::doMove(GameBoard::Move m, GameBoard* board) {
	switch (m.moveType)
	{
	case draw:
		board->drawCardsFromStockPile(1);
		break;
	case colToCol:
		if (!board->moveCardsFromColtoCol(m.fromCol, m.row, m.toCol)) {
			getAvailableMoves();
		}
		break;
	case colToFoundation:
		board->placeInFoundationFromCol(m.fromCol);
		break;
	case drawToFoundation:
		board->placeInFoundationFromDraw();
		break;
	case foundationToCol:
		board->removeFromFoundation(m.toCol, m.fromCol);
		break;
	case drawToCol:
		board->moveCardFromDrawn(m.toCol);
		break;
	}
}