#include "CardColumn.h"

CardColumn::CardColumn() {
	this->head = nullptr;
	this->tail = nullptr;
	this->firstVisable = nullptr;
	this->posOfFistVis = -1;
	this->length = 0;
	this->altColour = true;
	this->ascendingOrder = false;
}

CardColumn::CardColumn(bool asc,bool altColour) {
		this->head = nullptr;
		this->tail = nullptr;
		this->firstVisable = nullptr;
		this->posOfFistVis = -1;
		this->length = 0;
		this->altColour = altColour;
		this->ascendingOrder = asc;
}

void CardColumn::push(Card* c) {
	if (!c->isShown() && this->posOfFistVis != -1)
		return;//all cards after a shown card have to be shown.
	Node* newNode=new Node();
	newNode->value = c;
	newNode->next = nullptr;
	newNode->prev = this->tail;
	if (c->isShown()&&(this->length == 0 || this->posOfFistVis ==-1) ) {
		this->posOfFistVis = this->length;
		this->firstVisable = newNode;
	}
	if (this->length == 0) {
		this->head = newNode;
	}
	else {
		this->tail->next = newNode;
	}
	this->tail = newNode;
	this->length++;
}

void CardColumn::push(Node* n,int numOfCard) {
	if (n != nullptr) {
		n->prev = this->tail;
		if (isEmpty()) {
			this->head = n;
			this->firstVisable = n;
			this->posOfFistVis = 0;
		}
		else {
			this->tail->next = n;
		}
		Node* cur = n;
		while (cur != nullptr) {
			if (cur->next == nullptr)
				this->tail = cur;
			cur = cur->next;
		}
		this->length += numOfCard;
	}
	
}

Card* CardColumn::pop() {
	if (isEmpty()) {
		return nullptr;
	}
	this->length--;
	Card* c = this->tail->value;
	this->tail = this->tail->prev;
	if (this->length <= 1) {
		this->head = this->tail;

	}
	if (isEmpty()) {
		this->head = nullptr;
		return c;
	}

	
	this->tail->next = nullptr;
	this->tail->value->reveal();
	if (this->length <= this->posOfFistVis) {
		this->posOfFistVis = this->length-1;
		this->firstVisable = this->tail;
	}

	return c;
}

CardColumn::Node* CardColumn::popGroupFrom(int n) {
	if (n < this->posOfFistVis) {
		return nullptr;
	}
	if (isValidStack(n)) {
		Node* cur = this->firstVisable;
		int pos = this->posOfFistVis;
		while (cur != nullptr) {
			if (pos == n) {
				this->tail = cur->prev;
				length -= abs(length - pos);
				if (this->tail != nullptr) {
					this->tail->next = nullptr;
					this->tail->value->reveal();
					if (this->length <= this->posOfFistVis) {
						this->posOfFistVis = this->length-1;
						this->firstVisable = this->tail;
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
	int pos=this->posOfFistVis;
	Node* cur = this->firstVisable;
	while (cur != nullptr)
	{
		if (pos++ == n) {
			return *cur->value;
		}
		cur = cur->next;
	}
	return Card();
}

Card CardColumn::peek() {
	return *this->firstVisable->value;
}

bool CardColumn::contains(Card card) {
	Node* cur=this->firstVisable;
	while (cur!=nullptr)
	{
		if (cur->value->isEqual(card)) {
			return true;
		}
		cur = cur->next;
	}
	return false;
}

bool CardColumn::isValidStack(int start) {
	if (start < this->posOfFistVis)
		return false;//should throw error
	Node* cur = this->firstVisable;
	Node* prev = this->firstVisable;
	int pos = this->posOfFistVis;
	while (cur!=nullptr)
	{
		if (pos > start && !prev->value->doesStack(*cur->value, this->altColour, this->ascendingOrder)) {
			return false;
		}
		prev = cur;
		cur = cur->next;
		pos++;
	}
	return true;
}

string CardColumn::toString() {
	string s= "";
	Node* cur = this->head;
	while (cur!=nullptr)
	{
		if (cur->value->isShown()) {
			s += cur->value->toShortString();
		}
		else {
			s += "####";
		}
		if(cur->next!=nullptr)
			s += ",";
		cur = cur->next;
	}
	return s;
}