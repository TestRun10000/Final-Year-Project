#pragma once
#include"Card.h"
#include<memory>

class CardColumn
{
private:
	typedef struct Node {
		struct Node* prev;
		struct Node* next;
		Card* value;
	} Node;

	Node* head;
	Node* tail;
	Node* firstVisable;
	int posOfFistVis;
	int length;
	bool ascendingOrder;
	bool altColour;

public:
	CardColumn();

	CardColumn(bool asc, bool altColour);

	~CardColumn() {
		delete head;
		delete tail;
		delete firstVisable;
	}

	Node* getHead() { return this->head; }

	Node* getTail() { return this->tail; }

	int getLength() { return this->length; }

	int getPosOfFirstVis() { return this->posOfFistVis; }

	bool isEmpty() { return this->length == 0; }

	//append a single card
	void push(Card* c);

	//append a stack of cards
	void push(Node* n,int numOfCards);

	//removes the tail from list and returns it
	Card* pop();

	//removes a valid stack starting at n from colums and returns the first item of the stack returns null if no valid stack
	Node* popGroupFrom(int n);

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


	CardColumn(const CardColumn& other)
	{
		this->head = nullptr;
		this->tail = nullptr;
		this->firstVisable = nullptr;
		this->length = 0;
		this->posOfFistVis = other.posOfFistVis;
		this->ascendingOrder = other.ascendingOrder;
		this->altColour = other.altColour;

		Node* current = other.head;
		Node* prevNewNode = nullptr;

		while (current != nullptr)
		{
			Node* newNode = new Node{ nullptr, nullptr, current->value };

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

