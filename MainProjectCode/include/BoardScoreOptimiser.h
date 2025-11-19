#pragma once
#include"BoardScorer.h"	
#include"DummyPlayer.h"
#include<tuple>
#include<fstream>
#include<random>
#include<array>
#include <sstream>

class BoardScoreOptimiser
{
private:
	int populationSize = 120;
	float mutationRate = 0.001;
	std::vector<BoardScorer> population;
	std::vector<float> fitnessValues;
	std::mt19937 gen;


public:
	BoardScoreOptimiser(float mutationRate=0.4) {
		this->populationSize = 30;
		this->mutationRate = mutationRate;
		this->population = vector<BoardScorer>();
		this->fitnessValues = vector<float>();; 
	}

	void grow();
private:


	//shift values random amount based on mutation rate
	void mutate(BoardScorer* boardS);

	//one point crossover
	std::vector<BoardScorer> crossover(BoardScorer a, BoardScorer b);

	//Tournament selection
	BoardScorer selection();

	void reseedPopulation();

	float fittnes(BoardScorer b);

	BoardScorer getFittest() {
		float base = this->fitnessValues.at(0);
		int index = 0;
		for (int i = 1; i < this->populationSize; i++) {
			if (base < this->fitnessValues.at(i)) {
				index = i;
				base = this->fitnessValues.at(i);
			}
		}
		return this->population.at(index);
	}

	void SaveGeneration() {
		std::ofstream file((std::string)RESOURCES_PATH + "pop.csv");
		for (int i = 0; i < this->populationSize; i++) {
			file << this->population.at(i).toString()<<this->fitnessValues.at(i)<<"\n";
		}
		file.close();
	}

	bool loadPopulation() {
		std::ifstream file((std::string)RESOURCES_PATH + "pop.csv");
		std::string text;
		int index=0;
		if (!file.is_open()) return false;
		while (std::getline(file,text)) {
			std::stringstream ss(text);
			std::string cell;
			int count = 0;
			BoardScorer bs=BoardScorer();
			while (std::getline(ss, cell, ',')) {
				if (count < 5) {
					bs.setWeight(count, std::stof(cell));
				}
				else {
					this->fitnessValues.push_back(std::stof(cell));
				}
				count++;
			}
			this->population.push_back(bs);
		}
		std::cout<<"Population Loaded"<<endl;
		file.close();
		return true;
	}

};

