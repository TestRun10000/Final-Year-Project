#pragma once
#include<string>
#include<algorithm>

using namespace std;


class Card
{
public:
	enum Suits
	{
		Clubs, Spades, Hearts, Diamonds
	};

	enum Colours
	{
		red, black
	};

private:
	Suits suit;
	short rank;
	Colours colour;
	bool isVisable = false;

public:
	Card();
	Card(Suits suit,unsigned short rank);
	Card(Card const&) = default;

	Suits getSuit() { return suit; }
	short getRank() { return rank; }
	Colours getColour() { return colour; }

	string toString();

	string toShortString();

	//are cards same rank suit and colour
	bool isEqual(Card b);
	//only compares rank ignores suit and colour 0 if equal 1 if b is greater
	int compareRank(Card b);
	
	//can card b go ontop of this card is descending/ascending order of same suit or different colour
	bool doesStack(Card b, bool altColour = true,bool asc=false);

	bool isShown() { return isVisable; }

	void hide() { isVisable = false; }
	void reveal() { isVisable = true; }
	//null type for card this is the defualt type
	bool isBlank();

	~Card();
};



