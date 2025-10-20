#pragma once
#include "CardColumn.h"
#include<string>
class CardColumnCollection
{
private:
	CardColumn columns[7];
	int size = 7;

public:
	CardColumnCollection() {
		//this->columns =(CardColumn**) calloc(7, sizeof(CardColumn*));
		for (int i = 0; i < 7; i++) { this->columns[i] =  CardColumn(); }
		//this->size = 7;
	}

	//~CardColumnCollection() {
	//	for (int i = 0; i < 7; i++) {
	//		this->columns[i].~CardColumn();
	//	}
	//}

	//CardColumnCollection(int size,bool ascOrder,bool altColour) {
	//	this->columns =(CardColumn**) calloc(size, sizeof(CardColumn*));
	//	for (int i = 0; i < size; i++) { this->columns[i] = new CardColumn(ascOrder,altColour); }
	//	this->size = size;
	//}

	CardColumnCollection(CardColumnCollection const&) = default;

	int getLengthOfCol(unsigned int c) {
		if (c > this->size) return 0;
		return this->columns[c].getLength();
	}

	int getPosOfFistVisOfCol(unsigned int c) {
		if (c > this->size) return -1;
		return this->columns[c].getPosOfFirstVis();
	}

	std::string colString(unsigned int c) {
		if (c > this->size) return "";
		return this->columns[c].toString();
	}

	//moves cards or card stack between columns
	bool moveCardsFromColtoCol(unsigned int fromCol, unsigned int fromRow, unsigned int toCol, bool allowAnyToEmpty = false);
	
	//returns card value at the bottom of a column
	Card getBottomCardFromColumn(unsigned int column);
	
	//add a card to a column
	void pushCardToColumn(unsigned int column,Card card);

	//removes the last card from the column returns pointer to that card
	Card popFromColumn(unsigned int column,bool dontReveal = false);

	//removes a complete collection from a column starting at a set row. returns pointer to the top card of the collection. does not return collection.
	Card popCollectionFromColumn(unsigned int column, unsigned int row, bool dontReveal = false);

	//get first visable from column 
	Card getFirstVisFromCol(unsigned int col);

	void hideFirstVisOfCol(unsigned int col);

	std::string toString();

	bool isColEmpty(unsigned int col);

};

