#pragma once
#include<stack>
#include"Card.h"
#include<memory>

class Deck
{
private:
	Card deck[52];
	int topOFDeck = -1;


public:

	Deck() = default;
	Deck(int numSuits);
	Deck(Deck const &d) {
		topOFDeck = d.topOFDeck;
		std::copy(d.deck, d.deck + 52, deck);
	}

	bool isEmpty() { return this->topOFDeck == -1; }

	bool isFull() { return this->topOFDeck == 51; }

	int numInDeck() { return this->topOFDeck+1; }

	Card draw();

	Card peek() { return deck[this->topOFDeck]; }

	void push(Card c);

	//uses Fisher-Yates shuffle
	void shuffle();

	void printDeck();
};

