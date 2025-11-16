#pragma once
#include"PPOHelpers.h"
#include"DummyPlayer.h"
#include<array>
#include<torch/torch.h>
#include<torch/optim.h>
#include<vector>
#include<fstream>
//Welford's algorithm
class MeanStdFilter {
private:
    // Tensors to store running mean, variance, and count
    torch::Tensor M;
    torch::Tensor S;
    torch::Tensor N;

public:
    MeanStdFilter(int shape,torch::Device device) {
        M = torch::zeros({ shape }, torch::kFloat32).to(device);
        S = torch::ones({ shape }, torch::kFloat32).to(device) * 1e-4;
        N = torch::ones({ 1 }, torch::kFloat32).to(device) * 1e-4; 
    }

    void update(const torch::Tensor& x, torch::Device device) {
        if (x.sizes().empty() || x.sizes()[0] <= 1) {
            return;
        }
        torch::Tensor batch_mean = x.mean(0,false);
        torch::Tensor batch_var = x.var(0,false,false);
        float batch_count = (float)x.sizes()[0];

        float total_count = N.item<float>() + batch_count;
        torch::Tensor delta = batch_mean - M;
        torch::Tensor new_M = M + delta * batch_count / total_count;
        torch::Tensor new_S = S + batch_var * batch_count +
            delta * delta * N * batch_count / total_count;

        M = new_M;
        S = new_S;
        N = torch::tensor({ total_count }).to(device);
    }



    torch::Tensor mean() const { return M; }
    torch::Tensor std() const {
        torch::Tensor raw_std = torch::sqrt(S / N + 1e-8);
        return torch::clamp(raw_std, 0.1f, 1e9f);
    }
};


class PPO
{
private:
	//board is 146 array and there is 773 unique moves possible
    NetDeep actor = NetDeep(146, 904, 512);
    NetDeep critic = NetDeep(146, 1, 512);
	float gamma = 0.999;
    float lambda = 0.95;
	int updatesPerLoop = 8;
	int numberLoops = 1000;
	float clip = 0.3;
	float Actlr = 0.0003;
	float Crtlr = 0.0003;
    int batchSize = 64;
    float entropy_co = 0.01;
    bool winFlag = false;
    int test = 0;
	torch::optim::Adam actorOptim= torch::optim::Adam(this->actor.get()->parameters(), torch::optim::AdamOptions(this->Actlr));
	torch::optim::Adam criticOptim= torch::optim::Adam(this->critic.get()->parameters(), torch::optim::AdamOptions(this->Crtlr));
	std::ofstream logger= std::ofstream("C:/Users/jonat/source/25-26_CE301_sale_jonathan/SolitairCPPCM/Resources/RLLogger.csv", std::ios_base::app);
    torch::Device device = torch::kCPU;
    MeanStdFilter obs_normalizer = MeanStdFilter(146, this->device);

	advantegeTrajectorie rollout(bool Dummy=false);

    std::vector<std::vector<float>> computeRewardsToGo(std::vector<std::vector<float>> rewards); 
	BatchActionProbPair getActionBatch(torch::Tensor state, std::vector<std::vector <GameBoard::Move>> moves);
    BatchActionProbPair getActionBatchDummy(std::vector<DummyPlayer> games,torch::Tensor states, std::vector<std::vector <GameBoard::Move>> moves);
    ActionProbPair getAction(torch::Tensor state, vector < GameBoard::Move> moves);

	std::vector<float> advantageGAE(std::vector<std::vector<float>>rewards, std::vector <std::vector<torch::Tensor>> states);

    trajectorie flatten(std::vector <std::vector<torch::Tensor>> states, 
        std::vector <std::vector<int>> actions, std::vector<std::vector<float>> rewardsTwoD, std::vector <std::vector<float>> actionProb);


	torch::Tensor evaluateReward(torch::Tensor obs);
	
	torch::Tensor evaluateProb(torch::Tensor obs,torch::Tensor act);

    void LoadActor(NetDeep model, std::string path);
    void LoadCritic(NetDeep model,std::string path);

	void LogData(float totalReward, float Actorloss,float CriticLoss,float AdvantageMean,float MeanReward);

public:
	void learn();

	void Test();
    PPO() {
        if (torch::cuda::is_available()) {
            this->device = torch::kCUDA;
            std::cout<<"using cuda" << std::endl;
        }
        this->actor->to(this->device);
        this->critic->to(this->device);
        this->obs_normalizer = MeanStdFilter(146, this->device);
    }
};

