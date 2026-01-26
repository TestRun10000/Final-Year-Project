#pragma once
#include"PPOHelpers.h"
#include"DummyPlayer.h"
#include<array>
#include<torch/torch.h>
#include<torch/optim.h>
#include<vector>
#include<fstream>

#define inputSize 94
#define outputSize 905

class PPO
{
private:
	//board is 146 array and there is 773 unique moves possible but encoding to 904 is cleaner
    NetDeep actor = NetDeep((int)inputSize, (int)outputSize, 512);
    NetDeep critic = NetDeep((int)inputSize, 1, 512);
	float gamma = 0.99;
    float lambda = 0.90;
	int updatesPerLoop = 24;
	int numberLoops = 1750;
	float clip = 0.25;
    float Actlr = 0.0003;
	float Crtlr = 0.0005;
    int batchSize = 30;
    float entropy_co = 0;
    bool newNet = true;
    int winCount = 0;
    int test = 0;
	torch::optim::SGD actorOptim= torch::optim::SGD(this->actor.get()->parameters(), torch::optim::SGDOptions(this->Actlr));
	torch::optim::SGD criticOptim= torch::optim::SGD(this->critic.get()->parameters(), torch::optim::SGDOptions(this->Crtlr));
	std::ofstream logger= std::ofstream((std::string)RESOURCES_PATH +"RLLogger.csv", std::ios_base::app);
    torch::Device device = torch::kCPU;


	advantegeTrajectorie rollout(bool Dummy=false,int batchSize=1);

    std::vector<std::vector<float>> computeRewardsToGo(std::vector<std::vector<float>> rewards); 
	BatchActionProbPair getActionBatch(torch::Tensor state, std::vector<std::vector <GameBoard::Move>> moves);
    BatchActionProbPair getActionBatchDummy(std::vector<DummyPlayer> games,torch::Tensor states, std::vector<std::vector <GameBoard::Move>> moves);
    ActionProbPair getAction(torch::Tensor state, vector < GameBoard::Move> moves);

	std::vector<float> advantageGAE(std::vector<std::vector<float>>rewards, std::vector <std::vector<torch::Tensor>> states,float rewardMean, float rewardVar);
    //torch::Tensor GAES(torch::Tensor reward, torch::Tensor states, std::vector<int> batches);



    trajectorie flatten(std::vector <std::vector<torch::Tensor>> states, 
        std::vector <std::vector<int>> actions, std::vector<std::vector<float>> rewardsTwoD, std::vector <std::vector<float>> actionProb);


	torch::Tensor evaluateReward(torch::Tensor obs);
	
	torch::Tensor evaluateProb(torch::Tensor obs,torch::Tensor act);

    void LoadActor(NetDeep model, std::string path);
    void LoadCritic(NetDeep model,std::string path);

	void LogData(float totalReward, float Actorloss,float CriticLoss,float AdvantageMean,float MeanReward,float kl,float entropy,float explainedVar);

public:
	void learn();

	float Test();
    PPO() {
        if (torch::cuda::is_available()) {
            this->device = torch::kCUDA;
            std::cout<<"using cuda" << std::endl;
        }
        LoadActor(this->actor, (std::string)RESOURCES_PATH + "actor.pth");
        LoadCritic(this->critic, (std::string)RESOURCES_PATH + "critic.pth");
        this->actor->to(this->device);
        this->critic->to(this->device);
     
    }
};

