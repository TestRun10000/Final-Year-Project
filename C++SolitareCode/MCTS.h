#pragma once
#include"BoardNode.h"
#include<vector>
#include<thread>
#include<mutex>
#include<limits>
class MCTS
{
private:
	std::mutex node_mutex;
	BoardNode* root;
	int depthLimit=3;
	int iterations=500;
	int veriance=100;
	float c = 1.414213562;//root 2
	BoardNode* current;
	vector <GameBoard::Move> currentActionSet;
public:
	MCTS();

	MCTS(GameBoard board);
	MCTS(GameBoard board,set<Card> seen, GameBoard::Move lastMove);

	~MCTS() {
		delete root;
	}

private:
	BoardNode* selection(BoardNode* base);
	BoardNode *expand();
public:
	GameBoard::Move getAction();


};

