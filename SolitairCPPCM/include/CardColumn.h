#pragma once
#include"Card.h"
#include<memory>
#ifdef _DEBUG
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
// Replace _NORMAL_BLOCK with _CLIENT_BLOCK if you want the
// allocations to be of _CLIENT_BLOCK type
#else
#define DBG_NEW new
#endif

class Node {
public:
	weak_ptr<Node> prev;
	shared_ptr<Node> next;
	Card value;

	Node(shared_ptr<Node> prev = nullptr, shared_ptr<Node> next = nullptr, Card c = Card()) {
		this->prev = prev;
		this->next = next;
		this->value = c;
	}
};


class CardColumn
{
private:
	shared_ptr<Node> head;
	shared_ptr<Node> tail;
	shared_ptr<Node> firstVisable;
	int posOfFistVis;
	int length;
	bool ascendingOrder;
	bool altColour;

public:
	CardColumn();

	CardColumn(bool asc, bool altColour);

	~CardColumn() {
		//Node* cur = this->head;
		//while (cur != nullptr) {
		//	Node* next = cur->next;
		//	delete cur;
		//	cur = next;
		//}
	}

	Node* getHead() { return this->head.get(); }

	Node* getTail() { return this->tail.get(); }

	int getLength() { return this->length; }

	int getPosOfFirstVis() { return this->posOfFistVis; }

	bool isEmpty() { return this->length == 0; }

	//append a single card
	void push(Card c);

	//append a stack of cards
	void push(shared_ptr<Node> n, int numOfCards);

	//removes the tail from list and returns it
	Card pop(bool dontReveal = false);

	//removes a valid stack starting at n from colums and returns the first item of the stack returns null if no valid stack
	shared_ptr<Node> popGroupFrom(int n, bool dontReveal = false);

	//get visable card from column
	Card get(int n);

	//return the first Visable Card
	Card peek();

	//contains visable card
	bool contains(Card card);

	//is group starting at n a valid stack for this column
	bool isValidStack(int start);

	//returns coloumn as a list seperated by columns.
	string toString();

	void hideTopCard();

	CardColumn(const CardColumn& other)
	{
		this->head = nullptr;
		this->tail = nullptr;
		this->firstVisable = nullptr;
		this->length = 0;
		this->posOfFistVis = other.posOfFistVis;
		this->ascendingOrder = other.ascendingOrder;
		this->altColour = other.altColour;

		shared_ptr<Node> current = other.head;
		shared_ptr<Node> prevNewNode = nullptr;

		while (current != nullptr)
		{
			shared_ptr<Node> newNode = make_shared<Node>(nullptr, nullptr, current->value);
			if (!this->head)
				this->head = newNode;

			if (prevNewNode)
				prevNewNode->next = newNode;

			newNode->prev = prevNewNode;
			prevNewNode = newNode;

			if (current == other.firstVisable)
				this->firstVisable = newNode;

			current = current->next;
		}

		this->tail = prevNewNode;
		this->length = other.length;
	}
};

