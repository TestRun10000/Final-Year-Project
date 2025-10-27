#include "MCTS.h"

MCTS::MCTS() {
	GameBoard board=GameBoard();
	board.randomDeal();
	set<Card> unseen = set<Card>();
	for (int i = 0; i < 4; i++)
		for (int j = 1; j <= 13; j++)
			unseen.insert(Card((Card::Suits)i, j));
	set<Card> seen = board.visableCards();
	std::set<Card> newUnseeen;

	std::set_difference(
		unseen.begin(), unseen.end(),
		seen.begin(), seen.end(),
		std::inserter(newUnseeen, newUnseeen.begin())
	);
	this->root =new BoardNode(nullptr, board, newUnseeen, GameBoard::Move(),0);
	this->current = root;
	this->currentActionSet = this->current->actionSet();
}

MCTS::MCTS(GameBoard board) {
	set<Card> unseen = set<Card>();
	for (int i = 0; i < 4; i++) {
		for (int j = 1;j <= 13; j++)
			unseen.insert(Card((Card::Suits)i, j));
	}
	set<Card> seen = board.visableCards();
	std::set<Card> newUnseeen;

	std::set_difference(
		unseen.begin(), unseen.end(),
		seen.begin(), seen.end(),
		std::inserter(newUnseeen, newUnseeen.begin())
	);
	this->root =new BoardNode(nullptr, board, newUnseeen, GameBoard::Move(),0);
	this->current = root;
	this->currentActionSet = this->current->actionSet();
}

MCTS::MCTS(GameBoard board, set<Card> seen,GameBoard::Move lastMove) {
	set<Card> unseen = set<Card>();
	for (int i = 0; i < 4; i++) {
		for (int j = 1; j <= 13; j++)
			unseen.insert(Card((Card::Suits)i, j));
	}
	std::set<Card> newUnseeen;

	std::set_difference(
		unseen.begin(), unseen.end(),
		seen.begin(), seen.end(),
		std::inserter(newUnseeen, newUnseeen.begin())
	);
	this->root = new BoardNode(nullptr, board, newUnseeen, GameBoard::Move(), 0);
	this->root->setLastMove(lastMove);
	this->current = root;
	this->currentActionSet = this->current->actionSet();
	this->veriance = std::exp(-(double)seen.size()) * 100;
}

BoardNode* MCTS::selection(BoardNode* base) {
	BoardNode* x = base;
	float bestVal = -std::numeric_limits<float>::infinity();
	if (base->getChildren().empty()) {
		return base;
	}
	for (auto node : base->getChildren()) {
		if (node->UCB(this->c) > bestVal) {
			x = node;
			bestVal = node->UCB(this->c);
		}
	}
	return selection(x);
}


BoardNode* MCTS::expand() {
	int actionsIndex=this->current->getChildren().size();
	actionsIndex = actionsIndex >= this->currentActionSet.size() ? actionsIndex % this->currentActionSet.size() : actionsIndex;
	GameBoard newBoard = GameBoard();
	newBoard.partailDeal(this->current->getUnseenCard(), this->current->getCurrentBoard());
	BoardNode* newNode=new BoardNode(this->current, newBoard, this->current->getUnseenCard(), this->currentActionSet.at(actionsIndex),this->current->getDepth() + 1);
	this->current->expand(newNode);
	return newNode;
}

GameBoard::Move MCTS::getAction() {
	BoardNode* selected = this->current;
	for (int i = 0; i < this->iterations; i++) {
		if (this->current->getChildren().size() == this->currentActionSet.size() * this->veriance) {
			this->current = selection(this->root);
			this->currentActionSet = this->current->actionSet();
		}
		if (this->current->getDepth() < this->depthLimit) {
		}
		selected=expand();
		selected->Rollout();
	}
	BoardNode* x = this->root;
	int bestVal = -std::numeric_limits<int>::infinity();
	for (auto node : this->root->getChildren()) {
		if (node->getReward() > bestVal) {
			x = node;
			bestVal = node->getReward();
		}
	}
	//if (this->root->getMoveScore()==x->getMoveScore()) {
	//	return GameBoard::generateMove(0,0,0,0);
	//}
	return x->getAction();
}