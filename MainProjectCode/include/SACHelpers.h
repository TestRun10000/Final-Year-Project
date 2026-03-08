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

	trajectorie(torch::Tensor state, int action, torch::Tensor reward, torch::Tensor nextState, bool done, torch::Device device=torch::kCPU) {
		this->state = state.clone().to(device);
		this->action = torch::tensor(action,torch::kLong).to(device);
		this->reward = reward.clone().to(device);
		this->nextState = nextState.clone().to(device);
		this->done = torch::tensor(done,torch::kFloat).to(device);
		
	}
	trajectorie() {}
};	

struct trajectorieTensorBatch {
	torch::Tensor state;
	torch::Tensor action;
	torch::Tensor reward;
	torch::Tensor nextState;
	torch::Tensor done;

	trajectorieTensorBatch(std::vector<torch::Tensor> state, std::vector<torch::Tensor>  action, std::vector<torch::Tensor>  reward, std::vector<torch::Tensor>  nextState, std::vector<torch::Tensor>  done) {
		this->state = torch::stack(state);
		this->action = torch::stack(action);
		this->reward = torch::stack(reward);
		this->nextState = torch::stack(nextState);
		this->done = torch::stack(done);

	}
	trajectorieTensorBatch() {}
};


class ReplayBuffer {
private:
	std::vector<trajectorie> buffer;
	int maxSize;
	int index;
	int size;
	std::mt19937 gen;

public:
	ReplayBuffer(int maxSize) {
		this->buffer = std::vector<trajectorie>(maxSize);
		this->index = 0;
		this->size = 0;
		this->maxSize = maxSize;
	}

	void push(trajectorie tra) {
		this->buffer.at(index) = tra;
		trajectorie store = trajectorie(tra.state.clone(), tra.action.item<int>(), tra.reward.clone(), tra.nextState.clone(), tra.done.item<bool>());
		int newIndex = this->index + 1 >= this->maxSize ? 0:this->index + 1;
		this->index = newIndex;
		this->size = this->size + 1 >= maxSize ? maxSize : this->size + 1;
		//std::cout << "index:" << newIndex << std::endl;
	}

	std::vector<const trajectorie*> getSample(int sampleSize) {
		std::vector<const trajectorie*> sample = std::vector<const trajectorie*>();
		for (int i = 0; i < sampleSize; i++) {
			std::uniform_int_distribution<> chance(0, this->size-1);
			int pos = chance(gen);
			sample.push_back(&this->buffer.at(pos));
		}
		
		return sample;
	}

	int getSize() { return this->size; }

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