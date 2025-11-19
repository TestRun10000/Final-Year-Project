#include "CardColumnCollection.h"
#include <iostream>

bool CardColumnCollection::moveCardsFromColtoCol(unsigned int fromCol, unsigned int fromRow, unsigned int toCol,bool allowAnyToEmpty) {
	if (fromCol >= 7 || fromRow < this->columns[fromCol].getPosOfFirstVis() || toCol >= 7) return false;
	int count = abs((int)fromRow - (int)this->columns[fromCol].getLength());
	if (count > 1) {
		Card topOfStack = this->columns[fromCol].get(fromRow);
		if (this->columns[toCol].isEmpty() || this->columns[toCol].getPosOfFirstVis() == -1) {
			if (topOfStack.getRank() == 13||allowAnyToEmpty) {
				this->columns[toCol].push(this->columns[fromCol].popGroupFrom(fromRow,allowAnyToEmpty), count);
				return true;
			}

		}
		else {
			if (this->columns[toCol].getTail()->value.doesStack(topOfStack)) {
				this->columns[toCol].push(this->columns[fromCol].popGroupFrom(fromRow,allowAnyToEmpty), count);
				return true;
			}
		}
	}
	else {
		Card card = this->columns[fromCol].getTail()->value;
		if (card.isBlank()) return false;
		if (this->columns[toCol].isEmpty()||this->columns[toCol].getPosOfFirstVis() == -1) {
			if (card.getRank() == 13 || allowAnyToEmpty); {
				this->columns[toCol].push(card);
				this->columns[fromCol].pop(allowAnyToEmpty);
				return true;
			}
		}
		else {
			
			if (this->columns[toCol].getTail()->value.doesStack(card)) {
				this->columns[toCol].push(card);
				this->columns[fromCol].pop();
				return true;
			}

		}
	}
	cout << "Move Failed" << endl;
	cout << "To:" << toCol << " From:" << fromCol << "," << fromRow << endl;
	return false;
}
 
Card CardColumnCollection::getBottomCardFromColumn(unsigned int col) {
	if(col>this->size) return Card();
	return this->columns[col].getTail()->value;
}

Card CardColumnCollection::getFirstVisFromCol(unsigned int col) {
	if (col > this->size) return Card();
	return this->columns[col].peek();
}

void CardColumnCollection::pushCardToColumn(unsigned int column,Card card) {
	if (column > this->size) return;
	this->columns[column].push(card);
}

Card CardColumnCollection::popFromColumn(unsigned int column, bool dontReveal) {
	if (column > this->size) return Card();
	if (this->columns[column].getLength() == 0) return Card();
	return this->columns[column].pop(dontReveal);
}

Card CardColumnCollection::popCollectionFromColumn(unsigned int column, unsigned int row, bool dontReveal) {
	if (column > this->size) return Card();
	if (this->columns[column].getLength() == 0) return  Card();
	return this->columns[column].popGroupFrom(row,dontReveal)->value;
}

bool CardColumnCollection::isColEmpty(unsigned int col) {
	if (col > this->size) return true;
	return this->columns[col].isEmpty();
}

void CardColumnCollection::hideFirstVisOfCol(unsigned int col) {
	if (col > this->size) return;
	this->columns[col].hideTopCard();
}

string CardColumnCollection::toString() {
	int maxRow = 0;
	
	string* colsStrings=(string*)calloc(this->size,sizeof(string));
	string out;
	for (int i = 0; i < this->size; i++) {
		colsStrings[i] = this->columns[i].toString();
		if (maxRow < (colsStrings[i].length() + 1) / 5) {//character len of cards is 4 comma adds to length no comma at end so shifted count by 1
			maxRow = (colsStrings[i].length() + 1) / 5;
		}
	}
	for (int i = 0; i < maxRow; i++) {
		int cardStart = (i * 5);
		for (int j = 0; j < this->size; j++) {
			string line = i < (colsStrings[j].length() + 1) / 5 ? colsStrings[j].substr(cardStart, 4) : string(4, ' ');
			out = out + line;
			if (j < 6) {
				out = out + " : ";
			}

		}
		out = out + "\n ";
	}
	free(colsStrings);
	return out;
}