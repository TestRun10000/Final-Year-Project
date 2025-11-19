#pragma once
#include<torch/torch.h>
#include<array>
#include<cmath>
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
	torch::Tensor states;
	torch::Tensor actions;
	torch::Tensor rewards;
	torch::Tensor actionProb;
	int size;

	trajectorie(std::vector<torch::Tensor> states, std::vector<int> actions, std::vector<float> reward, std::vector<float> actionProb,torch::Device device) {
		this->size = states.size();
		this->states = torch::stack(states).to(device);
		this->actions = torch::tensor(actions,torch::kInt).to(device);
		this->rewards = torch::tensor(reward,torch::kFloat).to(device);
		this->actionProb= torch::tensor(actionProb,torch::kFloat).to(device);
		
	}
	trajectorie() {}
};	


struct advantegeTrajectorie {
	torch::Tensor advantage;
	trajectorie traject;
};

struct ActionProbPair {
	int action;
	float actionProb;
};

struct BatchActionProbPair {
	torch::Tensor actions;
	torch::Tensor log_probs;
};