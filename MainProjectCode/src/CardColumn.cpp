#include "CardColumn.h"
#include<iostream>

CardColumn::CardColumn() {
	this->head = nullptr;
	this->tail = nullptr;
	this->firstVisable = nullptr;
	this->posOfFistVis = -1;
	this->length = 0;
	this->altColour = true;
	this->ascendingOrder = false;
}

CardColumn::CardColumn(bool asc, bool altColour) {
	this->head = nullptr;
	this->tail = nullptr;
	this->firstVisable = nullptr;
	this->posOfFistVis = -1;
	this->length = 0;
	this->altColour = altColour;
	this->ascendingOrder = asc;
}

void CardColumn::push(Card c) {
	if (!c.isShown() && this->posOfFistVis != -1)
		return;//all cards after a shown card have to be shown.
	shared_ptr<Node> shared = make_shared<Node>(this->tail, nullptr, c);
	if (c.isShown() && (this->length == 0 || this->posOfFistVis == -1)) {
		this->posOfFistVis = this->length;
		this->firstVisable = shared;
	}
	if (this->length == 0) {
		this->head = shared;
	}
	else {
		this->tail->next = shared;
	}
	this->tail = shared;
	this->length++;
}

void CardColumn::push(shared_ptr<Node> n, int numOfCard) {
	if (n != nullptr) {
		shared_ptr<Node> lockedTail = this->tail;
		n->prev = lockedTail;
		if (isEmpty()) {
			this->head = n;
			this->firstVisable = n;
			this->posOfFistVis = 0;
		}
		else if (this->posOfFistVis == -1) {
			this->firstVisable = n;
			this->posOfFistVis = this->length;
			lockedTail->next = n;
		}
		else {
			lockedTail->next = n;
		}
		shared_ptr<Node> cur = n;
		while (cur != nullptr) {
			this->length++;
			if (cur->next == nullptr)
				this->tail = cur;
			cur = cur->next;
		}
		//this->length += numOfCard;
	}

}

Card CardColumn::pop(bool dontReveal) {
	if (isEmpty()) {
		return Card();
	}
	this->length--;
	Card c = this->tail->value;
	this->tail = this->tail->prev.lock();
	if (this->length <= 1) {
		this->head = this->tail;

	}
	if (isEmpty()) {
		this->head = nullptr;
		this->posOfFistVis = -1;
		this->firstVisable = nullptr;
		return c;
	}


	this->tail->next = nullptr;
	if (!dontReveal) {
		this->tail->value.reveal();
	}
	if (this->length <= this->posOfFistVis) {

		this->posOfFistVis = this->tail->value.isShown() ? this->length - 1 : -1;
		this->firstVisable = this->posOfFistVis == -1 ? nullptr : this->tail;
	}
	return c;
}

shared_ptr<Node> CardColumn::popGroupFrom(int n, bool dontReveal) {
	if (n < this->posOfFistVis) {
		return nullptr;
	}
	if (isValidStack(n)) {
		shared_ptr<Node> cur = this->firstVisable;
		int pos = this->posOfFistVis;
		while (cur != nullptr) {
			if (pos == n) {
				this->tail = cur->prev.lock();
				length -= abs(length - pos);
				if (this->tail != nullptr) {
					this->tail->next = nullptr;
					if (!dontReveal) {
						this->tail->value.reveal();
					}
					if (this->length <= this->posOfFistVis) {
						this->posOfFistVis = this->tail->value.isShown() ? this->length - 1 : -1;
						this->firstVisable = this->posOfFistVis == -1 ? nullptr : this->tail;
					}
				}
				if (this->isEmpty()) {
					this->head = this->tail;
				}
				return cur;
			}
			cur = cur->next;
			pos++;
		}
	}
	return nullptr;
}

Card CardColumn::get(int n) {
	if (n < this->posOfFistVis)
		return Card();
	int pos = this->posOfFistVis;
	shared_ptr<Node> cur = this->firstVisable;
	while (cur != nullptr)
	{
		if (pos++ == n) {
			return cur->value;
		}
		cur = cur->next;
	}
	return Card();
}

Card CardColumn::peek() {
	if (this->firstVisable == nullptr) {
		return Card(); // return default card or throw
	}
	return this->firstVisable->value;
}

bool CardColumn::contains(Card card) {
	shared_ptr<Node> cur = this->firstVisable;
	while (cur != nullptr)
	{
		if (cur->value.isEqual(card)) {
			return true;
		}
		cur = cur->next;
	}
	return false;
}

bool CardColumn::isValidStack(int start) {
	if (start < this->posOfFistVis)
		return false;//should throw error
	shared_ptr<Node> cur = this->firstVisable;
	shared_ptr<Node> prev = this->firstVisable;
	int pos = this->posOfFistVis;
	while (cur != nullptr)
	{
		if (pos > start && !prev->value.doesStack(cur->value, this->altColour, this->ascendingOrder)) {
			return false;
		}
		prev = cur;
		cur = cur->next;
		pos++;
	}
	return true;
}

//list only allows visable cards to be place ontop of other visable cards
void CardColumn::hideTopCard() {
	this->firstVisable->value.hide();;
	this->firstVisable = this->firstVisable->next;
	if (this->firstVisable == nullptr) {
		this->posOfFistVis = -1;
	}
	else {
		this->posOfFistVis++;
	}
}

string CardColumn::toString() {
	string s = "";
	shared_ptr<Node> cur = this->head;
	while (cur != nullptr)
	{
		if (cur->value.isShown()) {
			s += cur->value.toShortString();
		}
		else {
			s += "####";
		}
		if (cur->next != nullptr)
			s += ",";
		cur = cur->next;
	}
	return s;
}