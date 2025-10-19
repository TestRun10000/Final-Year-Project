#pragma once
#include"KlondikeBoard.h"
class BoardScorer
{
private:
	float scoreWeights[5];
public:
	BoardScorer() {
		this->scoreWeights[0] = 30.0f;
		this->scoreWeights[1] = 10.0f;
		this->scoreWeights[2] = 8.0f;
		this->scoreWeights[3] = 9.0f;
		this->scoreWeights[4] = 10.0f;
	}

	BoardScorer(float weights[5]) {
		this->scoreWeights[0] = weights[0];
		this->scoreWeights[1] = weights[1];
		this->scoreWeights[2] = weights[2];
		this->scoreWeights[3] = weights[3];
		this->scoreWeights[4] = weights[4];
	}

	float boardScore(GameBoard board) {
		float score = 0;
		score += this->scoreWeights[0] * board.numOfCardsInFoundation();
		score += this->scoreWeights[1] * board.numOfCardsRevealed();
		score += this->scoreWeights[2] * (24 - board.numCardInDeck());
		score += this->scoreWeights[3] * board.numOfEmptyCols();
		score += this->scoreWeights[4] * board.numOfColsStartingWithKing();
		return score;
	}

	void setWeight(unsigned int i,float weight) {
		if (i > 4) return;
		this->scoreWeights[i] = weight;
	}

	float getWeight(unsigned int i) {
		if (i > 4) return 0;
		return this->scoreWeights[i];
	}

	std::string toString() {
		std::string s = "";
		for (float var : this->scoreWeights)
			s += std::to_string(var)+",";
		return s;
	}
};

