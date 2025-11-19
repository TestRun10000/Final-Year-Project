#include "Card.h"
#include<iostream>

Card::Card() {
	this->rank = -1;
	this->colour = red;
	this->suit = Hearts;
}

Card::Card(Suits _suit, unsigned short _rank) {

	this->rank = _rank % 14 == 0 && _rank!=0 ? 1 : _rank;
	this->suit = _suit;
	if (suit < 2)
		this->colour = black;
	else
		this->colour = red;

}

string Card::toString() {
	string srank;
	switch (this->rank)
	{
	case -1:
		return "Blank";
	case 0:
		return "Joker";
		
	case 1:
		srank = "Ace";
		break;
	case 11:
		srank = "Jack";
		break;
	case 12:
		srank = "Queen";
		break;
	case 13:
		srank = "King";
		break;
	default:
		srank = to_string(this->rank);
	}
	switch (this->suit)
	{
	case Card::Clubs:
		return srank + "-Clubs";
	case Card::Spades:
		return srank + "-Spades";
	case Card::Diamonds:
		return srank + "-Diamonds";
		break;
	default:
		return srank + "-Hearts";
	}
}

string Card::toShortString() {
	string srank;
	switch (this->rank)
	{
	case -1:
		return "[]";
	case 0:
		return "JK";
	case 1:
		srank = " A";
		break;
	case 10:
		srank = "10";
		break;
	case 11:
		srank = " J";
		break;
	case 12:
		srank = " Q";
		break;
	case 13:
		srank = " K";
		break;
	default:
		srank = " " + to_string(this->rank);

	}
	switch (this->suit)
	{
	case Card::Clubs:
		return srank + "-C";
	case Card::Spades:
		return srank + "-S";
	case Card::Diamonds:
		return srank + "-D";
		break;
	default:
		return srank + "-H";
	}
}

bool Card::isEqual(Card b) {
	return b.getRank() == this->rank && b.getSuit() == this->suit && b.getColour() == this->colour;
}

int Card::compareRank(Card b) {
	if (b.getRank() == this->rank)
		return 0;
	else if (b.getRank() > this->rank)
		return 1;
	return -1;
}

bool Card::doesStack(Card b, bool altColour, bool asc) {
	if (this->rank - b.getRank() != 1 + (asc * -2))//if asc is false, if becomes if(compareRank != -1)
		return false;
	if (altColour && b.getColour() != this->colour)
		return true;
	else if (!altColour && b.getColour() == this->colour)
		return true;
	return false;
}

bool Card::isBlank() {
	return this->rank == -1;
}

Card::~Card() {}

int Card::EncodeCard(Card c) {
	if (c.getRank() >= 1) {
		return c.getSuit() * 13 + c.getRank();
	}
	else {
		return c.getRank();
	}
}

Card Card::DecodedCard(int c) {
	if(c>=1){
		c -= 1;
		return Card((Card::Suits)(c / 13), (c % 13)+1);
	}
	else {
		return Card((Card::Suits)0, c);
	}
}