#pragma once
#include<torch/torch.h>
#include<array>
#include<cmath>
#include<random>

struct NetImpl : torch::nn::Module{
	NetImpl(int64_t In, int64_t Out,int64_t hidden=256) :linear1(register_module("linear1", torch::nn::Linear(In, hidden))),
		linear2(register_module("linear2", torch::nn::Linear(hidden, hidden))),
		linear3(register_module("linear3", torch::nn::Linear(hidden, Out))) {}
	torch::nn::Linear linear1;
	torch::nn::Linear linear2;
	torch::nn::Linear linear3;

	torch::Tensor forward(torch::Tensor input) {
		torch::Tensor act1 = torch::relu(linear1(input));
		torch::Tensor act2 = torch::relu(linear2(act1));
		return linear3(act2);
	}

};
struct NetDeepImpl : torch::nn::Module {
	NetDeepImpl(int64_t In, int64_t Out, int64_t hidden = 512) :linear1(register_module("linear1", torch::nn::Linear(In, hidden))),
		linear2(register_module("linear2", torch::nn::Linear(hidden, hidden))),
		linear3(register_module("linear3", torch::nn::Linear(hidden, hidden))),
		linear4(register_module("linear4", torch::nn::Linear(hidden, Out))) {
	}
	torch::nn::Linear linear1;
	torch::nn::Linear linear2;
	torch::nn::Linear linear3;
	torch::nn::Linear linear4;


	torch::Tensor forward(torch::Tensor input) {
		torch::Tensor act1 = torch::relu(linear1(input));
		torch::Tensor act2 = torch::relu(linear2(act1));
		torch::Tensor act3 = torch::relu(linear3(act2));
		return linear4(act3);
	}

};
TORCH_MODULE(Net);
TORCH_MODULE(NetDeep);

struct trajectorie {
	torch::Tensor state;
	torch::Tensor action;
	torch::Tensor reward;
	torch::Tensor nextState;
	torch::Tensor done;

	trajectorie(torch::Tensor state, int action, float reward, torch::Tensor nextState, bool done) {
		this->state = state;
		this->actions = torch::tensor(action,torch::kInt);
		this->rewards = torch::tensor(reward, torch::kFloat);
		this->nextState = nextState;
		this->done = torch::tensor(done,torch::kBool);
		
	}
	trajectorie() {}
};	

class ReplayBuffer {
private:
	std::array<trajectorie> buffer;
	int maxSize;
	int index;
	std::mt19937 gen;

public:
	ReplayBuffer(int maxSize) {
		this->buffer = std::Array<trajectorie, maxSize>();
		this->length = 0;
		this.maxSize = maxSize;
	}

	void push(trajectorie tra) {
		int newIndex = this->index + 1 > this->maxSize ? 0:this->index + 1;
		this->buffer.at(newIndex) = tra;
	}

	std::vector<trajectorie> getSample(int size) {
		std::vector<trajectorie> sample = std::vector<trajectorie>();
		for (int i = 0; i < size; i++) {
			std::uniform_int_distribution<> chance(0, index);
			sample.append(this->buffer.at(chance(gen)));
		}
		return sample;
	}

};



struct ActionProbPair {
	int action;
	float actionProb;
};

struct BatchActionProbPair {
	torch::Tensor actions;
	torch::Tensor log_probs;
};

struct LossPair {
	torch::Tensor lossA;
	torch::Tensor lossB;
};