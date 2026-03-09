#pragma once
#include"SACHelpers.h"
#include"DummyPlayer.h"
#include<array>
#include<torch/torch.h>
#include<torch/optim.h>
#include<vector>
#include<fstream>

#define inputSize 124
#define outputSize 905

class SAC
{
private:
    //board is 146 array and there is 773 unique moves possible but encoding to 904 is cleaner
    NetDeep actor = NetDeep((int)inputSize, (int)outputSize, 512);
    NetDeep criticA = NetDeep((int)inputSize, (int)outputSize, 512);
    NetDeep criticB = NetDeep((int)inputSize, (int)outputSize, 512);
    NetDeep criticTA = NetDeep((int)inputSize, (int)outputSize, 512);
    NetDeep criticTB = NetDeep((int)inputSize, (int)outputSize, 512);
    float gamma = 0.99;
    float lambda = 0.90;
    int updatesPerLoop = 7;
    int numberLoops = 1000;
    float lr = 0.0003;
    int rolloutBatchSize = 5;
    int batchSize = 300;
    float entropy_co = 0;
    bool newNet = true;
    int winCount = 0;
    float tau = 0.05;
    float target_entropy = 0.98 * -std::log(1.0f / (float)outputSize);
    torch::Tensor logAlpha = torch::tensor(std::log(0.1),torch::TensorOptions().dtype(torch::kFloat32).requires_grad(true));
    torch::Tensor alpha = logAlpha.exp();
    torch::optim::Adam alphaOp = torch::optim::Adam(std::vector<torch::Tensor>{logAlpha}, torch::optim::AdamOptions(this->lr));
    torch::optim::Adam actorOptim = torch::optim::Adam(this->actor.get()->parameters(), torch::optim::AdamOptions(this->lr));
    torch::optim::Adam criticAOp = torch::optim::Adam(this->criticA.get()->parameters(), torch::optim::AdamOptions(this->lr));
    torch::optim::Adam criticBOp = torch::optim::Adam(this->criticB.get()->parameters(), torch::optim::AdamOptions(this->lr));
    
    ReplayBuffer buffer = ReplayBuffer(50000);


	std::ofstream logger= std::ofstream((std::string)RESOURCES_PATH +"RLLogger.csv", std::ios_base::app);
    torch::Device device = torch::kCPU;


    std::vector<trajectorie> rollout(int batchSize=1, bool dummy = false);

    std::vector<std::vector<float>> computeRewardsToGo(std::vector<std::vector<float>> rewards); 
	BatchActionProbPair getActionBatch(torch::Tensor state, std::vector<std::vector <GameBoard::Move>> moves);
    ActionProbPair getAction(torch::Tensor state, vector < GameBoard::Move> moves);
    BatchActionProbPair getActionBatchDummy(std::vector<DummyPlayer> games, torch::Tensor states, std::vector<std::vector <GameBoard::Move>> moves);



    void softUpdate(NetDeep& target, NetDeep& local, double tau) {
        auto targetParams = target->parameters();
        auto localParams = local->parameters();

        for (size_t i = 0; i < targetParams.size(); ++i) {
            targetParams[i].data().copy_(
                tau * localParams[i].data() + (1.0 - tau) * targetParams[i].data()
            );
        }
    }

            //std::cout << "done" << std::endl;
    void soft_update_target_networks(double tau) {
        softUpdate(criticTA, criticA, tau);
        softUpdate(criticTB, criticB, tau);
    }

    trajectorieTensorBatch getSample(int size);

    torch::Tensor getQvalue(torch::Tensor state);

    LossPair getCriticLoss(trajectorieTensorBatch tra);
    torch::Tensor getActorLoss(trajectorieTensorBatch tra);
    torch::Tensor getEntropyLoss(trajectorieTensorBatch tra);

    void LoadNetwork(NetDeep model, std::string path);

	void LogData(float totalReward, float Actorloss,float CriticLoss,float AdvantageMean,float MeanReward,float kl,float entropy,float explainedVar);

public:
	void learn();

	float Test();

    SAC() {
        if (torch::cuda::is_available()) {
            this->device = torch::kCUDA;
            std::cout<<"using cuda" << std::endl;
        }
        LoadNetwork(this->actor, (std::string)RESOURCES_PATH + "actor.pth");
        LoadNetwork(this->actor, (std::string)RESOURCES_PATH + "criticA.pth");
        LoadNetwork(this->actor, (std::string)RESOURCES_PATH + "criticB.pth");
        soft_update_target_networks(this->tau);
        this->actor->to(this->device);
        this->criticA->to(this->device);
        this->criticB->to(this->device);
        this->criticTA->to(this->device);
        this->criticTB->to(this->device);
        if (this->newNet) {
            for (auto tra : this->rollout(50,true)) {
                this->buffer.push(tra);
            }
        }

    }
};

