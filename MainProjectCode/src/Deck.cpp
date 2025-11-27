#include"Deck.h"
#include <iostream>


Deck::Deck(int numSuits=4) {

	numSuits = min(4, numSuits);
	int start = numSuits < 3 ? 1:0;
	for (int i = start; i < start+numSuits; i++) {
		for (int j = TopCard; j >0; j--) {
			push(Card((Card::Suits)i, j));
		}
	}
}

Card Deck::draw() {
	if (isEmpty()) {
		return Card();
		//throw std::invalid_argument("Deck is Empty");
	}
	return this->deck[topOFDeck--];
}

void Deck::push(Card c) {
	if (this->topOFDeck < 51) {
		this->deck[++this->topOFDeck] = c;
		return;
	}
	//throw std::invalid_argument("Deck is Full");
}

void Deck::shuffle() {
	if(isEmpty()) throw std::invalid_argument("Deck is Empty");
	Card temp;
	for (int i = this->topOFDeck; i > 0; i--) {
		int j = (rand() % i) + 1;
		temp = this->deck[i];
		this->deck[i] = this->deck[j];
		this->deck[j] = temp;
	}
}

void Deck::printDeck() {
	int count = numInDeck();
	if (count == 0) std::cout << "Deck is Empty" << endl;
	for (int i = 0; i < count; i++) {
		std::cout << deck[i].toString()<< endl;
	}
}