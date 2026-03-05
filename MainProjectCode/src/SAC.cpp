#include <SAC.h>
#include<torch/nn.h>


ActionProbPair SAC::getAction(torch::Tensor state, vector < GameBoard::Move> moves) {
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

BatchActionProbPair SAC::getActionBatch(torch::Tensor states, std::vector<std::vector <GameBoard::Move>> moves) {
		
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


//trajectorie SAC::rollout(bool Dummy,int batchSize){
//	std::vector<std::vector<torch::Tensor>> states = std::vector <std::vector<torch::Tensor>>(batchSize);
//	std::vector<std::vector<int>> actions = std::vector <std::vector<int>>(batchSize);
//	std::vector<std::vector<float>> rewards = std::vector <std::vector<float>>(batchSize);
//	std::vector<std::vector<float>> actionsProb = std::vector <std::vector<float>>(batchSize);
//	std::vector<DummyPlayer> games= std::vector<DummyPlayer>(batchSize);
//	float s[5] = { 10.818811,1.636655,6.108121,0.444798,6.098790 };
//	BoardScorer bs = BoardScorer(s);
//	for (int i = 0; i < batchSize; ++i) {
//		games.at(i) = DummyPlayer(bs,0);
//	}
//	std::vector<float> prevRev=std::vector<float>(batchSize); 
//	std::vector<bool> dones= std::vector<bool>(batchSize, false); 
//	std::vector<int> movesTaken = std::vector<int>(batchSize, 0);
//	int active_envs = batchSize;
//	int movesMade = 0;
//	int finalVal = 0;
//	float rewardMean = 0.0;
//	int actionsTaken = 0;
//
//
//	while (active_envs>0 && movesMade<140)
//	{
//		std::vector<std::vector<GameBoard::Move>> currentBatchMoves;
//		std::vector<std::vector<int>> currentBatchStates;
//		std::vector<int> active_indices;
//		std::vector<DummyPlayer> activeGame;
//		for (int i = 0; i < batchSize;i++) {
//			if (!dones.at(i)) {
//				currentBatchStates.push_back(games.at(i).EncodeBoard());
//				currentBatchMoves.push_back(games.at(i).getAvailableMoves());
//				active_indices.push_back(i);
//				activeGame.push_back(games.at(i));
//				if (movesMade == 0) {
//					prevRev.at(i) = games.at(i).numOfCardsRevealed();
//				}
//			}
//		}
//		std::vector<int> allStates;
//		allStates.reserve(currentBatchStates.size() * (int)inputSize);
//		for (const auto& state_vector : currentBatchStates) {
//			for (int val : state_vector) {
//				allStates.push_back(val);
//			}
//		}
//		torch::Tensor batchStates_cpu = torch::tensor(allStates, torch::TensorOptions().dtype(torch::kFloat32)).view({ active_envs, (int)inputSize });
//	
//		torch::Tensor batchStates = batchStates_cpu.to(this->device);
//		BatchActionProbPair outputBatch;
//		torch::Tensor actionsBatch;
//
//		outputBatch = getActionBatch(batchStates, currentBatchMoves);
//		actionsBatch= outputBatch.actions.to(torch::kCPU).squeeze(1);
//		torch::Tensor probsBatch = outputBatch.log_probs.to(torch::kCPU);
//
//
//		int activeIndex = 0;
//		for (int i = 0; i < batchSize;i++) {
//			if (!dones.at(i)) {
//				int action = actionsBatch[activeIndex].item<int>();
//				float actionProb = probsBatch[activeIndex].item<float>();
//				float baseScore = games.at(i).boardScore();
//				games.at(i).doMove(GameBoard::DecodeMove(action));
//				//float reward = (games.at(i).boardScore() - baseScore)/10;
//				//reward = std::signbit(reward) ? -0.2:0.2;
//				float reward=0.0;
//				float currentRev = games.at(i).numOfCardsRevealed();
//				if (action == 1 || (action >= 8 && action < 14)) {//to foundation
//					reward += 1.0f;
//				}
//				else if (action >= 14 && action < 17){// removed card from deck
//					reward += 0.1f;
//				}
//				else if (prevRev.at(i) < currentRev) {//revealed a card
//					reward += 0.5f;
//				}
//				prevRev.at(i) = currentRev;
//				//if (action >= 2 && action < 8) {
//				//	reward -= 1.0f;
//				//}
//				if (currentBatchStates.at(activeIndex).size() != (int)inputSize) {
//					std::cout << "error";
//
//				}
//				torch::Tensor curState = torch::tensor(currentBatchStates.at(activeIndex), torch::kFloat32).to(this->device);
//				//torch::Tensor normalized_state = (curState - obs_mean) / (obs_std + 1e-8);
//				actionsTaken++;
//				states.at(i).push_back(curState);
//				actions.at(i).push_back(action);
//				rewards.at(i).push_back(reward);
//				actionsProb.at(i).push_back(actionProb);
//				rewardMean += reward;
//				if (games.at(i).isGameOver()) {
//					dones.at(i) = true;
//					active_envs--;
//					movesTaken.at(i) = movesMade;
//					rewardMean -= reward;
//					if (games.at(i).gameWon()) {
//						if (!Dummy) {
//							//std::cout << "Winner"<<std::endl;
//							winCount++;
//						}
//						rewardMean += 1;
//						rewards.at(i).back() = 20;
//						
//					}
//					else {
//						rewardMean -= 1;
//						rewards.at(i).back() = -15;
//					}
//				}
//				else if (movesMade == 139) {
//					rewardMean -= reward;
//					rewardMean -= 1;
//					rewards.at(i).back() = -15;
//					movesTaken.at(i) = movesMade;
//				}
//				activeIndex++;
//			}
//		}
//		
//		movesMade++;
//	}
//	rewardMean /= actionsTaken;
//	float var = 0.0;
//	for (auto i : rewards) for (auto j : i) 
//		var+=std::pow(j - rewardMean, 2);
//	var /= actionsTaken;
//	var=std::sqrt(var)+1e-8;
//	trajectorie tra = flatten(states, actions, computeRewardsToGo(rewards), actionsProb);
//	//torch::Tensor adv = torch::tensor(advantageGAE(rewards, states,rewardMean,var),torch::kFloat).to(this->device);
//
//	return tra;
//}


std::vector<std::vector<float>> SAC::computeRewardsToGo(std::vector<std::vector<float>> rewards) {
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





//trajectorie SAC::flatten(std::vector <std::vector<torch::Tensor>> statesTwoD,
//	std::vector <std::vector<int>> actionsTwoD, std::vector<std::vector<float>> rewardsTwoD, std::vector <std::vector<float>> nextStateTwoD,std::vector <std::vector<float>> doneTwoD) {
//	std::vector<torch::Tensor> states = std::vector<torch::Tensor>();
//	std::vector<int> actions = std::vector<int>();
//	std::vector<float> nextState = std::vector<float>();
//	std::vector<float> rewards = std::vector<float>();
//	std::vector<bool> done= std::vector<bool>();
//	for (int i = 0; i < actionsTwoD.size(); i++) {
//		for (int j = 0; j < statesTwoD.at(i).size(); j++) {
//			states.push_back(statesTwoD.at(i).at(j));
//			actions.push_back(actionsTwoD.at(i).at(j));
//			nextState.push_back(nextStateTwoD.at(i).at(j));
//			rewards.push_back(rewardsTwoD.at(i).at(j));
//			done.push_back(doneTwoD.at(i).at(j));
//		}
//	}
//	return trajectorie(states, actions, rewards, nextState, done, this->device);
//}

LossPair SAC::criticLoss(trajectorie tra) {
	torch::NoGradGuard no_grad;
	torch::Tensor raw = this->actor.get()->forward(tra.nextState);
	torch::Tensor actionProb = torch::softmax(raw, -1).to(this->device);
	torch::Tensor actionProbLog = torch::log_softmax(raw, -1).to(this->device);

	torch::Tensor next_q_values_target = this->criticTA.get()->forward(tra.nextState);
	torch::Tensor next_q_values_target2 = this->criticTB.get()->forward(tra.nextState)
		;
	torch::Tensor soft_state_values =torch::sum(actionProb * (torch::min(next_q_values_target, next_q_values_target2) - this->alpha * actionProbLog), 1);
	torch::Tensor nextQ = tra.reward + ~tra.done * this->gamma * soft_state_values;

	torch::Tensor soft_q = this->criticA.get()->forward(tra.state).gather(1, tra.action.unsqueeze(-1)).squeeze(-1);
	torch::Tensor soft_q2 = this->criticB.get()->forward(tra.state).gather(1, tra.action.unsqueeze(-1)).squeeze(-1);

	torch::nn::MSELoss mse_loss(torch::nn::MSELossOptions().reduction(torch::kNone));
	torch::Tensor critic_error = mse_loss(soft_q, next_q_values_target);
	torch::Tensor critic_error2 = mse_loss(soft_q2, next_q_values_target2);
	torch::Tensor criticLossA = critic_error.mean();
	torch::Tensor criticLossB = critic_error2.mean();
	return LossPair(criticLossA,criticLossB);
}

torch::Tensor SAC::actorLoss(trajectorie tra) {
	torch::Tensor raw = this->actor.get()->forward(tra.state);
	torch::Tensor actionProb = torch::softmax(raw, -1).to(this->device);
	torch::Tensor actionProbLog = torch::log_softmax(raw, -1).to(this->device);

	torch::Tensor q_values_local = this->criticA.get()->forward(tra.state);
	torch::Tensor q_values_local2 = this->criticB.get()->forward(tra.state);

	torch::Tensor inside_term =  torch::min(q_values_local, q_values_local2) - this->alpha * actionProbLog;
	torch::Tensor policyLoss = torch::sum((actionProb * inside_term), 1).mean();
	return policyLoss;
}

torch::Tensor SAC::entropyLoss(trajectorie tra) {
	torch::Tensor actionProbLog = torch::log_softmax(this->actor.get()->forward(tra.state), -1).to(this->device);
	return -(this->logAlpha * (actionProbLog + this->target_entropy).detach()).mean();
}



void SAC::LoadActor(NetDeep model,std::string path) {
	try {
		torch::load(model, path);
		std::cout << path+" model loaded successfully.\n";
		this->newNet = false;
	}
	catch (const c10::Error& e) {
		std::cout << path + " moded did not load.\n";
	}
}

void SAC::LoadCritic(NetDeep model, std::string path) {
	try {
		torch::load(model, path);
		std::cout << path + " model loaded successfully.\n";
		this->newNet = false;
	}
	catch (const c10::Error& e) {
		std::cout << path + " moded did not load.\n";
	}
}

void SAC::LogData(float totalReward, float Actorloss, float CriticLoss, float AdvantageMean, float MeanReward, float kl, float entropy, float explainedVar) {
	this->logger << "," << totalReward << "," << Actorloss << "," << CriticLoss << "," << AdvantageMean << ",";
	this->logger << MeanReward<<","<< kl << "," << entropy << "," << explainedVar <<","<<this->winCount << "\n";
	std::cout << totalReward << "," << Actorloss << "," << CriticLoss << "," << AdvantageMean << ",";
	std::cout<< MeanReward << "," << explainedVar <<"," << this->winCount << "," << ((float)this->winCount / ((float)this->batchSize*10)) * 100 << "\n";
}

void SAC::learn() {
	trajectorie winTra;
	//std::cout<<"|reward2Go|Actorloss|CriticLoss|AdvantageMean|MeanReward|KL|EntropyMean|Explained Var|Win Count"<<endl;
	std::cout<<"|reward2Go|Actorloss|CriticLoss|AdvantageMean|MeanReward|Explained Var|Win Count"<<endl;
	long double avgSumReward = 0;
	for (int j = 1; j < this->numberLoops+1; j++) {
		srand(1);//((int)j/500)+1
		torch::Tensor ak;
		trajectorie traActor = this->rollout(false,this->batchSize);


			

		}
		/*if (j > 1500) {
			this->clip -= 0.0002;
		}*/
	/*	this->Actlr -= 0.00000005;
		static_cast<torch::optim::AdamOptions&>(this->actorOptim.param_groups()[0].options())
//			.lr(this->Actlr);*/
//		//this->entropy_co -= 0.0045;
//		if (j % 10 == 0 ||(j<=10 && this->newNet)) {
//			LogData(torch::sum(rewards).item<float>()/this->batchSize, actorLossVal, 
//				criticLossVal, ak.sum().item<float>(), (avgSumReward / 10) / this->batchSize, KL, entropyMean, explainedVar);
//			avgSumReward = 0;
//			this->winCount = 0;
//			torch::save(this->actor, (std::string)RESOURCES_PATH + "actor.pth");
//			torch::save(this->critic, (std::string)RESOURCES_PATH + "critic.pth");
//		}
//	}
//	torch::save(this->actor, (std::string)RESOURCES_PATH + "actor.pth");
//	torch::save(this->critic, (std::string)RESOURCES_PATH + "critic.pth");
}

float SAC::Test() {

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