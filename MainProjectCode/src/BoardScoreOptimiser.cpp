#include "BoardScoreOptimiser.h"

void BoardScoreOptimiser::mutate(BoardScorer* boardS) {
	std::uniform_real_distribution<> mutChance(0.0, 1.0);
	std::uniform_real_distribution<> mutAmount(-1, 1.0);
	for (int i = 0; i < 5; i++) {
		if (mutChance(this->gen) < this->mutationRate) {
			boardS->setWeight(i, boardS->getWeight(i) + mutAmount(this->gen));
		}
	}

}

std::vector<BoardScorer>  BoardScoreOptimiser::crossover(BoardScorer a, BoardScorer b) {
	std::uniform_int_distribution<int> crossPointGen(1, 4);
	std::uniform_real_distribution<> crosspointOccurance(0, 1);
	int crossoverPoint = crossPointGen(this->gen);
	if (crosspointOccurance(this->gen) > 0.9)  {
		return { a,b };
	}
	float childA[5] = { 0,0,0,0,0 };
	float childB[5] = { 0,0,0,0,0 };
	for (int i = 0; i < 5; i++) {
		if (i < crossoverPoint) {
			childA[i] = a.getWeight(i);
			childB[i] = b.getWeight(i);
		}
		else {
			childA[i] = b.getWeight(i);
			childB[i] = a.getWeight(i);
		}
	}
	return { BoardScorer(childA),BoardScorer(childB) };
}

BoardScorer BoardScoreOptimiser::selection() {
	std::uniform_int_distribution<int> select(0.0,this->populationSize-1);
	int index = select(this->gen);
	float maxScore = this->fitnessValues.at(index);
	for (int i = 0; i < 3; i++) {
		int x= select(this->gen);
		if (maxScore < this->fitnessValues.at(x)) {
			index = x;
			maxScore = this->fitnessValues.at(x);
		}
	}
	return this->population.at(index);
}

void BoardScoreOptimiser::reseedPopulation() {
	std::vector<BoardScorer> newPop = std::vector<BoardScorer>();
	std::uniform_real_distribution<float> base(0, 10);
	newPop.push_back(this->getFittest());
	for (int i = 1; i < this->populationSize; i++) {
		if (i % 10 == 0) {
			float weights[5] = { base(this->gen), base(this->gen) ,base(this->gen) ,base(this->gen) ,base(this->gen) };
			newPop.push_back(BoardScorer(weights));
		} 
		else {
			newPop.push_back(selection());
		}
	}
	//free(this->population);
	this->population = newPop;
	for (int i = 1; i < this->populationSize-1; i+=2) {
		std::vector<BoardScorer> children = crossover(this->population.at(i-1), this->population.at(i));
		newPop.at(i) = children.at(0);
		newPop.at(i + 1) = children.at(1);
	}
	for (int i = 1; i < this->populationSize; i++) {
		mutate(&this->population.at(i));
	}
}

float BoardScoreOptimiser::fittnes(BoardScorer b) {
	int winCount = 0;
	srand(1);
	for (int i = 0; i < 100; i++) {
		DummyPlayer game(b,0);
		while (!game.isGameOver()) {
			game.makeBestMove();
		}
		if (game.gameWon()) {
			winCount++;
		}
	}
	return (winCount / (float)100)*100;
}

void BoardScoreOptimiser::grow() {
	//srand(time(0));
	std::uniform_real_distribution<float> base(0, 10);
	bool loaded = loadPopulation();
	if (!loaded) {
		//this->population.push_back(BoardScorer());
		//this->fitnessValues.push_back(0);
		for (int i = 0; i < this->populationSize; i++) {
			float weights[5] = { base(this->gen), base(this->gen) ,base(this->gen) ,base(this->gen) ,base(this->gen) };
			this->population.push_back(BoardScorer(weights));
			this->fitnessValues.push_back(0);
		}

	}
	std::ofstream file((std::string)RESOURCES_PATH + "genOut.csv", std::ios_base::app);
	std::ofstream best((std::string)RESOURCES_PATH + "best.txt", std::ios_base::app);
	std::ifstream genortation((std::string)RESOURCES_PATH + "gen.txt");
	int gen = 0;
	if (loaded) {
		if (genortation.is_open()) {
			std::string text;
			std::getline(genortation, text);
			gen = std::stoi(text);
		}
		else {
			std::cout << "Enter generation: ";
			std::cin >> gen;
		}
	}
	genortation.close();
	//file.open;
	int maxGen = gen + 10 ;
	for (gen = gen; gen < maxGen; gen++) {
		float maxFitness = 0;
		for (int j = 0; j < this->populationSize; j++) {
			std::cout << gen+1 << "/" << maxGen<<"\t" << j+1 << "/" << this->populationSize << " \r";
			std::cout.flush();
			float fit = fittnes(this->population.at(j));
			if (fit > maxFitness)
				maxFitness = fit;
			this->fitnessValues.at(j) = fit;
			//maxFitness += fit;
		}
		std::cout << "\r";
		best << this->getFittest().toString()<<"\n";
		reseedPopulation();
		std::cout.flush();
		file << gen <<"," << maxFitness<<"\n";
	}
	std::cout << this->population.at(0).toString() << endl;
	std::cout << fittnes(this->population.at(0)) << endl;
	std::ofstream geno((std::string)RESOURCES_PATH + "gen.txt");
	geno << std::to_string(maxGen);
	geno.close();
	file.close();
	SaveGeneration();
}

