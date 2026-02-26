#include "MCTS.h"

MCTS::MCTS() {
	GameBoard board = GameBoard();
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
	this->root = make_shared<BoardNode>(nullptr, board, newUnseeen, GameBoard::Move(), 0);
	this->current = root;
	this->currentActionSet = this->current->actionSet();
}

MCTS::MCTS(GameBoard board) {
	set<Card> unseen = set<Card>();
	for (int i = 0; i < 4; i++) {
		for (int j = 1; j <= 13; j++)
			unseen.insert(Card((Card::Suits)i, j));
	}
	set<Card> seen = board.visableCards();
	std::set<Card> newUnseeen;

	std::set_difference(
		unseen.begin(), unseen.end(),
		seen.begin(), seen.end(),
		std::inserter(newUnseeen, newUnseeen.begin())
	);
	this->root = make_shared<BoardNode>(nullptr, board, newUnseeen, GameBoard::Move(), 0);
	this->current = root;
	this->currentActionSet = this->current->actionSet();
}

MCTS::MCTS(GameBoard board, set<Card> seen, GameBoard::Move lastMove) {
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
	this->root = make_shared<BoardNode>(nullptr, board, newUnseeen, GameBoard::Move(), 0);
	this->root->setLastMove(lastMove);
	this->current = root;
	this->currentActionSet = this->current->actionSet();
	this->veriance = ceil(std::exp((-(double)seen.size() / this->rateOfDecay) + (7.0 / this->rateOfDecay)) * this->peakVeriance);
}

shared_ptr<BoardNode> MCTS::selection(shared_ptr<BoardNode> base) {
	shared_ptr<BoardNode> x = base;
	if (base.get() == this->root.get() && (base->getChildren().empty() || base->getChildren().size() < base->actionSet().size() * this->veriance)) {
		return base;
	}
	float bestVal = -std::numeric_limits<float>::infinity();
	if (base->getChildren().empty() || base->isTerminal()||base->getChildren().size() != base->actionSet().size() * (this->veriance / (base->getDepth() + 1))) {
		return base;
	}
	//if (x->getChildren().size() < x->actionSet().size() * this->veriance && !x->isTerminal()) {
	//	bestVal = x->UCB(this->c);
	//}

	for (auto node : base->getChildren()) {
		if (node->UCB(this->c) > bestVal) {
			x = node;
			bestVal = node->UCB(this->c);
		}
	}
	return selection(x);
	if (x == base) {
		return base;
	}
}


shared_ptr<BoardNode> MCTS::expand() {
	//int actionsIndex = (this->current->getChildren().size())/ (this->veriance / (this->current->getDepth() + 1));
	int actionsIndex = this->current->getChildren().size();
	actionsIndex = actionsIndex >= this->currentActionSet.size() ? actionsIndex % this->currentActionSet.size() : actionsIndex;
	GameBoard newBoard = GameBoard();
	newBoard.partailDeal(this->current->getUnseenCard(), this->current->getCurrentBoard());
	shared_ptr<BoardNode> newNode = make_shared<BoardNode>(this->current, newBoard, this->current->getUnseenCard(), this->currentActionSet.at(actionsIndex), this->current->getDepth() + 1);
	this->current->expand(newNode);
	return newNode;
}

GameBoard::Move MCTS::getAction() {
	int count = this->iterations;
	if (this->veriance < this->peakVeriance*0.8) { count = this->iterations / 2; }
	if (this->veriance < this->peakVeriance/2) { count = this->iterations / 3; }
	for (int i = 0; i < count; i++) {
		//std::cout << "Begin" << endl;
		if ((this->current->getChildren().size() >= this->current->actionSet().size() * (this->veriance / (this->current->getDepth() + 1) * this->percentToExpand))) {
			//std::cout << "Selection" << endl;
			this->current = selection(this->root);
			this->currentActionSet = this->current->actionSet();
		}
		if (this->current->isTerminal()) {
			//std::cout << "Terminal" << endl;
			int r = 0;
			this->current->Rollout(&r);
			this->current->Backpropergate(r);

		}
		else {
			shared_ptr<BoardNode> n = expand();
			int r = 0;
			n->Rollout(&r);
			n->Backpropergate(r);
		}
	
	}


	shared_ptr<BoardNode> x = this->root;
	int bestVal = -std::numeric_limits<int>::infinity();
	for (auto node : this->root->getChildren()) {
		if (node->getReward() > bestVal) {
			x = node;
			bestVal = node->getReward();
		}
	};
	return x->getAction();
}
