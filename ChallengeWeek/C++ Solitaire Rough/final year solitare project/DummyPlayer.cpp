#include "DummyPlayer.h"
#include <queue>
#include<vector>



void DummyPlayer::makeBestMove() {
	list<KlondikeGameController::Move> moves = getAvailableMoves();
	std::priority_queue<ActionValue, std::vector<ActionValue>, CompareTask> task_queue;
	for (KlondikeGameController::Move m : moves) {
		GameBoard copy = DummyPlayer::copyBoard();
		DummyPlayer::doMove(m, &copy);
		int score = copy.boardScore();
		task_queue.emplace(score, m);
	}
	KlondikeGameController::doMove(task_queue.top().move);
}

void DummyPlayer::makeRandomMove() {
	list<KlondikeGameController::Move> moves = getAvailableMoves();
	int j = (rand() % moves.size());
	KlondikeGameController::Move m = generateMove(0, 0, 0, 0);
	int x = 0;
	for (const auto& mov : moves) {
		if (x == j) {
			m = mov;
			break;
		}
		x++;
	}
	KlondikeGameController::doMove(m);
}

void DummyPlayer::suidRandomMove() {
	list<KlondikeGameController::Move> moves = getAvailableMoves();
	std::priority_queue<ActionValue, std::vector<ActionValue>, CompareTask> task_queue;
	float total = 0;
	for (KlondikeGameController::Move m : moves) {
		GameBoard copy = DummyPlayer::copyBoard();
		DummyPlayer::doMove(m, &copy);
		int score = copy.boardScore();
		task_queue.emplace(score, m);
		total += score;
	}
	float ran = (rand() % 76);//represents 75% or if that is how mouch stronger the best move is 
	ActionValue lastPair = task_queue.top();
	task_queue.pop();
	if (ran>(lastPair.score * 100) / total ){
		KlondikeGameController::doMove(lastPair.move);
		return;
	}
	for (int i = 1; i < task_queue.size()+1; i++) {
		ActionValue cur = task_queue.top();
		task_queue.pop();
		if (ran > (cur.score * 100) / total && ran <= (lastPair.score * 100) /total) {
			KlondikeGameController::doMove(cur.move);
			break;
		}
		lastPair = cur;
	}	
}


void DummyPlayer ::doMove(KlondikeGameController::Move m, GameBoard* board) {
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