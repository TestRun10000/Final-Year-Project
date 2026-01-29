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
	shared_ptr<BoardNode> root;
	int iterations=3000;
	int veriance=100;
	float c = 1.414213562;//root 2
	int rateOfDecay = 20;
	int peakVeriance = 150;
	shared_ptr<BoardNode> current;
	vector <GameBoard::Move> currentActionSet;
public:
	MCTS();

	MCTS(GameBoard board);
	MCTS(GameBoard board,set<Card> seen, GameBoard::Move lastMove);


private:
	shared_ptr<BoardNode> selection(shared_ptr<BoardNode> base);
	shared_ptr<BoardNode> expand();
public:
	GameBoard::Move getAction();


};

