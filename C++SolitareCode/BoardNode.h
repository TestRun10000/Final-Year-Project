#pragma once
#include"DummyPlayer.h"
#include<vector>
#include<cmath>
#include<set>
#include<algorithm>

class BoardNode
{
private:
	weak_ptr<BoardNode> parent;
	int numOfVists;
	int reward;
	int depth;
	list<shared_ptr<BoardNode>> children;
	
	KlondikeGameController board;
	GameBoard::Move action= GameBoard::Move();
	set<Card> unseenCard;

public:

	BoardNode() {
		this->numOfVists = 0;
		this->reward = 0;
		this->parent = std::weak_ptr<BoardNode>();
		this->board = KlondikeGameController();
		this->children = list<shared_ptr<BoardNode>>();
		this->unseenCard = set<Card>();
		this->action = GameBoard::Move();
		this->depth = 0;
	}

	BoardNode(shared_ptr<BoardNode> parent, GameBoard board,set<Card> unseenCard,GameBoard::Move action,int depth) {
		this->numOfVists = 0;
		this->reward = 0;
		this->parent = parent;
		this->board =KlondikeGameController(board);
		this->children = list<shared_ptr<BoardNode>>();;
		this->unseenCard = unseenCard;
		this->action = action;
		this->depth = depth;
	}

	float UCB(float c) {
		shared_ptr<BoardNode> parent = this->parent.lock();
		if (parent != nullptr)
			return (this->reward / (float)numOfVists) + (c * sqrt(log(parent->numOfVists) / this->numOfVists));
		return this->reward / (float)numOfVists;
	}

	vector<GameBoard::Move> actionSet() {
		return this->board.getAvailableMoves();
	}

	GameBoard::Move getAction() { return this->action; }

	int getReward() { return (this->reward/this->numOfVists)+this->board.boardScore(); }
	float getMoveScore() { return this->board.boardScore(); }

	int getNumOfVisits() { return this->numOfVists; }

	list<shared_ptr<BoardNode>> getChildren() { return this->children; }

	GameBoard getCurrentBoard() { return this->board.getBoard(); }

	set<Card> getUnseenCard(){return this->unseenCard;}
	
	int getDepth() { return this->depth; }

	bool isTerminal() {
		 return board.isGameOver();
	}

	void expand(shared_ptr<BoardNode> child) {
		this->children.push_back(child);
		child->board.doMove(child->action);
		set<Card> seen = child->board.visableCards();
		//child->reward += child->board.boardScore();
		std::set<Card> newUnseeen;

		std::set_difference(
			this->unseenCard.begin(), this->unseenCard.end(),
			seen.begin(), seen.end(),
			std::inserter(newUnseeen, newUnseeen.begin())
		);
		child->setUnseen(newUnseeen);
	}

	void setUnseen(set<Card> unseen) {
		this->unseenCard = unseen;
	}

	void Rollout(int* reward) {
		*reward = 0;
		GameBoard rboard = GameBoard();
		float baseScore = this->board.boardScore();
		DummyPlayer player(this->board.getBoard());
		while (!player.isGameOver()) {
			player.bestMoveRandom();
		}
		if (player.gameWon()) {
			*reward = 1000;
		}
		else {
			*reward= (int)(player.boardScore()-baseScore);


		}

	
	
		//this->Backpropergate(reward);
	}

	void Backpropergate(int reward) {
		this->numOfVists++;
		this->reward += reward;
		shared_ptr<BoardNode> parent = this->parent.lock();
		if (parent != nullptr) {
			parent->Backpropergate(reward);
		}
	}


	void setLastMove(GameBoard::Move m) {
		this->board.setLastMove(m);
	}

};

