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
	this->veriance = ceil(std::exp(( - (double)seen.size() / 20.0) + (7.0 / 20.0)) * 100);
}

BoardNode* MCTS::selection(BoardNode* base) {
	BoardNode* x = base;
	float bestVal = -std::numeric_limits<float>::infinity();
	if (x->getChildren().size() < x->actionSet().size() * this->veriance) {
		bestVal = x->UCB(this->c);
	}
	if (base->getChildren().empty()) {
		return base;
	}
	for (auto node : base->getChildren()) {
		if (node->UCB(this->c) > bestVal) {
			
				x = node;	
				bestVal = node->UCB(this->c);
			
		}
	}
	if (x == base) {
		return base;
	}
	if (x->getChildren().size() < x->actionSet().size() * this->veriance) {
		return x;
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
	for (int i = 0; i < this->iterations; i++) {
		//vector<thread> threads;
		//std::vector<int> rewards(10, 0);
		//vector<BoardNode*> nodes;
		if (!(this->current->getChildren().size() < this->current->actionSet().size() * (this->veriance/(this->current->getDepth()+1)))) {
			this->current = selection(this->root);
			this->currentActionSet = this->current->actionSet();
		}
		BoardNode* n = expand();
		int r = 0;
		n->Rollout(&r);
		n->Backpropergate(r);
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
	//cout << this->root->getChildren().size()<<"," << this->root->actionSet().size() << endl;
	return x->getAction();
}
