#include <PPO.h>
#include<torch/nn.h>


ActionProbPair PPO::getAction(torch::Tensor state, vector < GameBoard::Move> moves) {
	torch::Tensor mean = actor.get()->forward(state).to(this->device);

	std::vector<int> mask = std::vector<int>(outputSize);

	for (auto m : moves) {
		int v = GameBoard::EncodeMove(m);
		mask.at(v) = 1;
	}

	torch::Tensor tmask = torch::tensor(mask, torch::TensorOptions().dtype(torch::kBool)).to(this->device);
	mean = mean.masked_fill(~tmask, -1e9).to(this->device);
	int action = torch::multinomial(torch::softmax(mean, -1).to(this->device), 1).item<int>();

	return ActionProbPair(action, torch::log_softmax(mean, -1).to(this->device)[action].item<float>());

}

BatchActionProbPair PPO::getActionBatch(torch::Tensor states, std::vector<std::vector <GameBoard::Move>> moves) {
		
		torch::Tensor mean = actor.get()->forward(states).to(this->device);
		int N = states.size(0);
		std::vector<int> flat_mask_data(N * (int)outputSize, 0);
		for (int i = 0; i < N; ++i) {
			for (const auto& m : moves.at(i)) {
				int v = GameBoard::EncodeMove(m);
				flat_mask_data.at(i * (int)outputSize + v) = 1;
			}
		}
		torch::Tensor tBatchMask = torch::tensor(flat_mask_data,torch::TensorOptions().dtype(torch::kBool))
			.view({ N, (int)outputSize })
			.to(this->device);

		mean = mean.masked_fill(~tBatchMask, -1e9);
		torch::Tensor logs = torch::log_softmax(mean, -1).to(this->device);
		torch::Tensor actions = torch::multinomial(torch::softmax(mean, -1).to(this->device), 1).to(this->device);
		torch::Tensor log_probs = logs.gather(1, actions).squeeze(1);
		return BatchActionProbPair{ actions, log_probs };
}

BatchActionProbPair PPO::getActionBatchDummy(std::vector<DummyPlayer> games,torch::Tensor states, std::vector<std::vector <GameBoard::Move>> moves) {
	std::vector<int> actions = std::vector<int>();
	int N = states.size(0);
	for (auto game : games) {
		actions.push_back(GameBoard::EncodeMove(game.makeBestMove()));
	}

	torch::Tensor tact = torch::tensor(actions, torch::kInt).to(this->device);
	if (actions.empty()) {
		return BatchActionProbPair(torch::empty({ 0 }, torch::kInt), torch::empty({ 0 }, torch::kFloat32));
	}
	//std::cout << "working"<<test++ << endl;
	torch::Tensor tActions = torch::tensor(actions, torch::kInt).unsqueeze(1).to(this->device);

	std::vector<int> flat_mask_data(N * (int)outputSize, 0);
	for (int i = 0; i < N; ++i) {
		for (const auto& m : moves.at(i)) {
			int v = GameBoard::EncodeMove(m);
			flat_mask_data.at(i * (int)outputSize + v) = 1;
		}
	}
	torch::Tensor tBatchMask = torch::tensor(flat_mask_data, torch::TensorOptions().dtype(torch::kBool))
		.view({ N, (int)outputSize })
		.to(this->device);
	//std::cout << "working"<< endl;

	torch::Tensor mean = actor.get()->forward(states).to(this->device);
	mean = mean.masked_fill(~tBatchMask, -1e9);
	torch::Tensor logs = torch::log_softmax(mean, -1).to(this->device);
	torch::Tensor log_probs = logs.gather(1, tActions).squeeze(1);
	
	return BatchActionProbPair(tact, log_probs);
}

advantegeTrajectorie PPO::rollout(bool Dummy,int batchSize){
	std::vector<std::vector<torch::Tensor>> states = std::vector <std::vector<torch::Tensor>>(batchSize);
	std::vector<std::vector<int>> actions = std::vector <std::vector<int>>(batchSize);
	std::vector<std::vector<float>> rewards = std::vector <std::vector<float>>(batchSize);
	std::vector<std::vector<float>> actionsProb = std::vector <std::vector<float>>(batchSize);
	std::vector<DummyPlayer> games= std::vector<DummyPlayer>(batchSize);
	float s[5] = { 10.818811,1.636655,6.108121,0.444798,6.098790 };
	BoardScorer bs = BoardScorer(s);
	for (int i = 0; i < batchSize; ++i) {
		games.at(i) = DummyPlayer(bs,0);
	}
	std::vector<float> prevRev=std::vector<float>(batchSize); 
	std::vector<bool> dones= std::vector<bool>(batchSize, false); 
	std::vector<int> movesTaken = std::vector<int>(batchSize, 0);
	int active_envs = batchSize;
	int movesMade = 0;
	int finalVal = 0;
	float rewardMean = 0.0;
	int actionsTaken = 0;


	while (active_envs>0 && movesMade<140)
	{
		std::vector<std::vector<GameBoard::Move>> currentBatchMoves;
		std::vector<std::vector<int>> currentBatchStates;
		std::vector<int> active_indices;
		std::vector<DummyPlayer> activeGame;
		for (int i = 0; i < batchSize;i++) {
			if (!dones.at(i)) {
				currentBatchStates.push_back(games.at(i).EncodeBoard());
				currentBatchMoves.push_back(games.at(i).getAvailableMoves());
				active_indices.push_back(i);
				activeGame.push_back(games.at(i));
				if (movesMade == 0) {
					prevRev.at(i) = games.at(i).numOfCardsRevealed();
				}
			}
		}
		std::vector<int> allStates;
		allStates.reserve(currentBatchStates.size() * (int)inputSize);
		for (const auto& state_vector : currentBatchStates) {
			for (int val : state_vector) {
				allStates.push_back(val);
			}
		}
		torch::Tensor batchStates_cpu = torch::tensor(allStates, torch::TensorOptions().dtype(torch::kFloat32)).view({ active_envs, (int)inputSize });
	
		torch::Tensor batchStates = batchStates_cpu.to(this->device);
		BatchActionProbPair outputBatch;
		torch::Tensor actionsBatch;
		if (Dummy) {
			outputBatch = getActionBatchDummy(activeGame,batchStates,currentBatchMoves);
			actionsBatch = outputBatch.actions.to(torch::kCPU);
		}
		else {
			outputBatch = getActionBatch(batchStates, currentBatchMoves);
			actionsBatch= outputBatch.actions.to(torch::kCPU).squeeze(1);
		}
		torch::Tensor probsBatch = outputBatch.log_probs.to(torch::kCPU);


		int activeIndex = 0;
		for (int i = 0; i < batchSize;i++) {
			if (!dones.at(i)) {
				int action = actionsBatch[activeIndex].item<int>();
				float actionProb = probsBatch[activeIndex].item<float>();
				float baseScore = games.at(i).boardScore();
				games.at(i).doMove(GameBoard::DecodeMove(action));
				//float reward = (games.at(i).boardScore() - baseScore)/10;
				//reward = std::signbit(reward) ? -0.2:0.2;
				float reward=0.0;
				float currentRev = games.at(i).numOfCardsRevealed();
				if (action == 1 || (action >= 30 && action < 58)) {//to foundation
					reward += 1.0f;
				}
				else if (action >= 58 && action < 65){// removed card from deck
					reward += 0.1f;
				}
				else if (prevRev.at(i) < currentRev) {//revealed a card
					reward += 0.5f;
				}
				prevRev.at(i) = currentRev;
				//if (action >= 2 && action < 8) {
				//	reward -= 1.0f;
				//}
				if (currentBatchStates.at(activeIndex).size() != (int)inputSize) {
					std::cout << "error";

				}
				torch::Tensor curState = torch::tensor(currentBatchStates.at(activeIndex), torch::kFloat32).to(this->device);
				//torch::Tensor normalized_state = (curState - obs_mean) / (obs_std + 1e-8);
				actionsTaken++;
				states.at(i).push_back(curState);
				actions.at(i).push_back(action);
				rewards.at(i).push_back(reward);
				actionsProb.at(i).push_back(actionProb);
				rewardMean += reward;
				if (games.at(i).isGameOver()) {
					dones.at(i) = true;
					active_envs--;
					movesTaken.at(i) = movesMade;
					rewardMean -= reward;
					if (games.at(i).gameWon()) {
						if (!Dummy) {
							//std::cout << "Winner"<<std::endl;
							winCount++;
						}
						rewardMean += 1;
						rewards.at(i).back() = 20;
						
					}
					else {
						rewardMean -= 1;
						rewards.at(i).back() = -15;
					}
				}
				else if (movesMade == 139) {
					rewardMean -= reward;
					rewardMean -= 1;
					rewards.at(i).back() = -15;
					movesTaken.at(i) = movesMade;
				}
				activeIndex++;
			}
		}
		
		movesMade++;
	}
	rewardMean /= actionsTaken;
	float var = 0.0;
	for (auto i : rewards) for (auto j : i) 
		var+=std::pow(j - rewardMean, 2);
	var /= actionsTaken;
	var=std::sqrt(var)+1e-8;
	trajectorie tra = flatten(states, actions, computeRewardsToGo(rewards), actionsProb);
	torch::Tensor adv = torch::tensor(advantageGAE(rewards, states,rewardMean,var),torch::kFloat).to(this->device);

	return advantegeTrajectorie(adv,tra);
}


std::vector<std::vector<float>> PPO::computeRewardsToGo(std::vector<std::vector<float>> rewards) {
	std::vector<std::vector<float>> rewards2Go= std::vector<std::vector<float>>();
	for (int j = 0; j < rewards.size(); j++) {
		float discounted = 0;
		std::vector<float> batch = std::vector<float>(rewards.at(j).size());
		for (int i = rewards.at(j).size() - 1; i >= 0; --i) {
			discounted = rewards.at(j).at(i) + this->gamma * discounted;
			batch.at(i) = discounted;
		}
		rewards2Go.push_back(batch);
	}
	return rewards2Go;
}


std::vector<float> PPO::advantageGAE(std::vector<std::vector<float>> rewards, std::vector <std::vector<torch::Tensor>> states,float rewardMean, float rewardStd) {
	std::vector<float> GAE = std::vector<float>();
	for (int i = 0; i < rewards.size(); i++) {
		float gae = 0;
		std::vector<float> gameReturnsReverse; 
		for (int j = rewards.at(i).size() - 1; j >= 0; --j) {
			float next = 0.0;
			if (j + 1 < rewards.at(i).size()) {
				next = this->critic.get()->forward(states.at(i).at(j+1)).item<float>();
			}
			float v = this->critic.get()->forward(states.at(i).at(j)).item<float>();
				float delat = rewards.at(i).at(j) + this->gamma * next - v;
				gae = delat + (this->gamma * this->lambda * gae);
			gameReturnsReverse.push_back(gae); 
		}
		GAE.insert(GAE.end(), gameReturnsReverse.rbegin(), gameReturnsReverse.rend());
	}

	return GAE;
}



trajectorie PPO::flatten(std::vector <std::vector<torch::Tensor>> statesTwoD,
	std::vector <std::vector<int>> actionsTwoD, std::vector<std::vector<float>> rewardsTwoD, std::vector <std::vector<float>> actionProbTwoD) {
	std::vector<torch::Tensor> states = std::vector<torch::Tensor>();
	std::vector<int> actions = std::vector<int>();
	std::vector<float> actionsProb = std::vector<float>();
	std::vector<float> rewards = std::vector<float>();
	for (int i = 0; i < actionsTwoD.size(); i++) {
		for (int j = 0; j < statesTwoD.at(i).size(); j++) {
			states.push_back(statesTwoD.at(i).at(j));
			actions.push_back(actionsTwoD.at(i).at(j));
			actionsProb.push_back(actionProbTwoD.at(i).at(j));
			rewards.push_back(rewardsTwoD.at(i).at(j));
		}
	}
	return trajectorie(states, actions, rewards, actionsProb, this->device);
}


torch::Tensor PPO::evaluateReward(torch::Tensor obs) {
	return this->critic.get()->forward(obs).squeeze().to(this->device);
}

torch::Tensor PPO::evaluateProb(torch::Tensor obs, torch::Tensor act) {
	torch::Tensor logs = this->actor.get()->forward(obs);
	torch::Tensor probs = torch::softmax(logs, -1);      
	torch::Tensor log_probs = torch::log_softmax(logs, -1);
	log_probs = log_probs.gather(1, act.unsqueeze(1)).squeeze(1);
	return log_probs.to(this->device);
}

void PPO::LoadActor(NetDeep model,std::string path) {
	try {
		torch::load(model, path);
		std::cout << path+" model loaded successfully.\n";
		this->newNet = false;
	}
	catch (const c10::Error& e) {
		std::cout << path + " moded did not load.\n";
	}
}

void PPO::LoadCritic(NetDeep model, std::string path) {
	try {
		torch::load(model, path);
		std::cout << path + " model loaded successfully.\n";
		this->newNet = false;
	}
	catch (const c10::Error& e) {
		std::cout << path + " moded did not load.\n";
	}
}

void PPO::LogData(float totalReward, float Actorloss, float CriticLoss, float AdvantageMean, float MeanReward, float kl, float entropy, float explainedVar) {
	this->logger << "," << totalReward << "," << Actorloss << "," << CriticLoss << "," << AdvantageMean << ",";
	this->logger << MeanReward<<","<< kl << "," << entropy << "," << explainedVar <<","<<this->winCount << "\n";
	std::cout << totalReward << "," << Actorloss << "," << CriticLoss << "," << AdvantageMean << ",";
	std::cout<< MeanReward << "," << explainedVar <<"," << this->winCount << "," << ((float)this->winCount / ((float)this->batchSize*10)) * 100 << "\n";
}

void PPO::learn() {
	trajectorie winTra;
	//std::cout<<"|reward2Go|Actorloss|CriticLoss|AdvantageMean|MeanReward|KL|EntropyMean|Explained Var|Win Count"<<endl;
	std::cout<<"|reward2Go|Actorloss|CriticLoss|AdvantageMean|MeanReward|Explained Var|Win Count"<<endl;
	long double avgSumReward = 0;
	for (int j = 1; j < this->numberLoops+1; j++) {
		srand(1);//((int)j/500)+1
		torch::Tensor ak;
		advantegeTrajectorie traActor = this->rollout(false,this->batchSize);
		//advantegeTrajectorie traDummy = this->rollout(true,1);
		torch::Tensor rewards = traActor.traject.rewards;//torch::cat({ traActor.traject.rewards,traDummy.traject.rewards }, 0)
		torch::Tensor actions = traActor.traject.actions;//torch::cat({ traActor.traject.actions,traDummy.traject.actions }, 0);
		torch::Tensor states = traActor.traject.states;// torch::cat({ traActor.traject.states,traDummy.traject.states }, 0);
		torch::Tensor prob = traActor.traject.actionProb;//torch::cat({ traActor.traject.actionProb,traDummy.traject.actionProb }, 0);


		avgSumReward += torch::sum(rewards).item<float>();
		//torch::Tensor Norm_rewards = (rewards - rewards.mean()) / (rewards.std() + 1e-8);
		ak = traActor.advantage;//torch::cat({traActor.advantage,traDummy.advantage},0);
		torch::Tensor crtTarget = ak;
		ak = (ak - ak.mean()) / (ak.std() + 1e-8);
		float actorLossVal=0.0;
		float criticLossVal=0.0;
		float entropyMean=0.0;
		float explainedVar = 0.0;
		float KL = 0.0;
		for (int i = 0; i < this->updatesPerLoop; i++) {
			torch::Tensor curPobs = this->evaluateProb(states, actions).to(this->device);
			torch::Tensor ratio = torch::exp(curPobs - prob).to(this->device);
			torch::Tensor surr1 = ratio * ak;
			torch::Tensor surr2 = torch::clamp(ratio, 1 - this->clip, 1 + this->clip).to(this->device) * ak;
			torch::Tensor logs = this->actor.get()->forward(states).to(this->device);
			torch::Tensor probs = torch::softmax(logs, -1).to(this->device);
			torch::Tensor entropy = (-probs * torch::log_softmax(logs, -1)).sum(-1).mean();
			entropyMean = entropy.item<float>();
			KL = torch::mean(prob - curPobs).item<float>();
			torch::Tensor actorLoss = (torch::min(surr1, surr2)).to(this->device).mean();// - this->entropy_co * entropy;
			actorLossVal = actorLoss.item<float>();
			this->actorOptim.zero_grad();
			actorLoss.backward({},true);
			//torch::nn::utils::clip_grad_norm_(this->actor->parameters(), 0.5);
			this->actorOptim.step();
			//crtTarget = V.detach()+crtTarget.detach();
			//torch::Tensor normCrtTarget = (crtTarget - crtTarget.mean()) / (crtTarget.std() + 1e-8);
			//torch::nn::HuberLoss()(V, rewards.detach());
			torch::Tensor V = this->evaluateReward(states).to(this->device);
			//torch::Tensor target = torch::ones(V.sizes()) * 5;
			torch::Tensor criticLoss = torch::nn::MSELoss()(V, rewards.detach());
			//
			
			criticLossVal = criticLoss.item<float>();
			this->criticOptim.zero_grad();
			criticLoss.backward();
			this->criticOptim.step();
			explainedVar = 1.0 - ((rewards - V).detach().var() / rewards.detach().var()).item<float>();
			

		}
		/*if (j > 1500) {
			this->clip -= 0.0002;
		}*/
	/*	this->Actlr -= 0.00000005;
		static_cast<torch::optim::AdamOptions&>(this->actorOptim.param_groups()[0].options())
			.lr(this->Actlr);*/
		//this->entropy_co -= 0.0045;
		if (j % 10 == 0 ||(j<=10 && this->newNet)) {
			LogData(torch::sum(rewards).item<float>()/this->batchSize, actorLossVal, 
				criticLossVal, ak.sum().item<float>(), (avgSumReward / 10) / this->batchSize, KL, entropyMean, explainedVar);
			avgSumReward = 0;
			this->winCount = 0;
			torch::save(this->actor, (std::string)RESOURCES_PATH + "actor.pth");
			torch::save(this->critic, (std::string)RESOURCES_PATH + "critic.pth");
		}
	}
	torch::save(this->actor, (std::string)RESOURCES_PATH + "actor.pth");
	torch::save(this->critic, (std::string)RESOURCES_PATH + "critic.pth");
}

float PPO::Test() {

	int winCount=0;
	
	for (int i = 0; i < 1000;i++) {
		std::cout << i << "/" << 1000 << " \r";
		std::cout.flush();
		KlondikeGameController game(0);
		while (!game.isGameOver())
		{
			torch::Tensor curState = torch::tensor(game.EncodeBoard(), torch::kFloat32).to(this->device);
			ActionProbPair action = getAction(curState, game.getAvailableMoves());
			game.doMove(GameBoard::DecodeMove(action.action));
			
		}
		if (game.gameWon()) {
			winCount++;

		}
	}
	std::cout << "Games Won: " << winCount << endl;
	std::cout << "This is a Win rate of:" << (winCount / (float)1000) * 100 << "%" << endl<<endl;
	return (winCount / (float)1000) * 100;
}