#include <PPO.h>
#include<torch/nn.h>
//based on code from git repo linked below
//https://github.com/ericyangyu/PPO-for-Beginners/blob/master/ppo.py
//todo add rewards return as trajactorie currently retturns At

ActionProbPair PPO::getAction(torch::Tensor state, vector < GameBoard::Move> moves) {
	torch::Tensor obs_mean = this->obs_normalizer.mean().to(this->device);
	torch::Tensor obs_std = this->obs_normalizer.std().to(this->device);
	torch::Tensor normalized_state = (state - obs_mean) / (obs_std + 1e-8);
	torch::Tensor mean = actor.get()->forward(normalized_state.unsqueeze(0)).to(this->device);

	std::vector<int> mask = std::vector<int>(904);

	for (auto m : moves) {
		int v = GameBoard::EncodeMove(m);
		mask.at(v) = 1;
	}

	torch::Tensor tmask = torch::tensor(mask, torch::TensorOptions().dtype(torch::kBool)).to(this->device);
	mean = mean.masked_fill(~tmask, -1e9).to(this->device);
	int action = torch::multinomial(torch::softmax(mean, -1).to(this->device), 1).item<int>();

	return ActionProbPair(action, torch::log_softmax(mean, -1).to(this->device)[0][action].item<float>());

}

BatchActionProbPair PPO::getActionBatch(torch::Tensor states, std::vector<std::vector <GameBoard::Move>> moves) {
		
		torch::Tensor mean = actor.get()->forward(states).to(this->device);
		int N = states.size(0);
		std::vector<int> flat_mask_data(N * 904, 0);
		for (int i = 0; i < N; ++i) {
			for (const auto& m : moves.at(i)) {
				int v = GameBoard::EncodeMove(m);
				flat_mask_data.at(i * 904 + v) = 1;
			}
		}
		torch::Tensor tBatchMask = torch::tensor(flat_mask_data,torch::TensorOptions().dtype(torch::kBool))
			.view({ N, 904 }) 
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
		if (!game.isGameOver()) {
			actions.push_back(GameBoard::EncodeMove(game.makeBestMove()));
		}
	}

	torch::Tensor tact = torch::tensor(actions, torch::kInt).to(this->device);
	if (actions.empty()) {
		return BatchActionProbPair(torch::empty({ 0 }, torch::kInt), torch::empty({ 0 }, torch::kFloat32));
	}
	std::cout << "working"<<test++ << endl;
	torch::Tensor tActions = torch::tensor(actions, torch::kInt).unsqueeze(1).to(this->device);

	std::vector<int> flat_mask_data(N * 904, 0);
	for (int i = 0; i < N; ++i) {
		for (const auto& m : moves.at(i)) {
			int v = GameBoard::EncodeMove(m);
			flat_mask_data.at(i * 904 + v) = 1;
		}
	}
	torch::Tensor tBatchMask = torch::tensor(flat_mask_data, torch::TensorOptions().dtype(torch::kBool))
		.view({ N, 904 })
		.to(this->device);
	std::cout << "working"<< endl;

	torch::Tensor mean = actor.get()->forward(states).to(this->device);
	mean = mean.masked_fill(~tBatchMask, -1e9);
	torch::Tensor logs = torch::log_softmax(mean, -1).to(this->device);
	torch::Tensor log_probs = logs.gather(1, tActions).squeeze(1);
	
	return BatchActionProbPair(tact, log_probs);
}

advantegeTrajectorie PPO::rollout(bool Dummy){
	std::vector<std::vector<torch::Tensor>> states = std::vector <std::vector<torch::Tensor>>(this->batchSize);
	std::vector<std::vector<int>> actions = std::vector <std::vector<int>>(this->batchSize);
	std::vector<std::vector<float>> rewards = std::vector <std::vector<float>>(this->batchSize);
	std::vector<std::vector<float>> actionsProb = std::vector <std::vector<float>>(this->batchSize);
	std::vector<DummyPlayer> games= std::vector<DummyPlayer>(this->batchSize);
	float s[5] = { 10.818811,1.636655,6.108121,0.444798,6.098790 };
	BoardScorer bs = BoardScorer(s);
	for (int i = 0; i < this->batchSize; ++i) {
		games.at(i) = DummyPlayer(bs,0);
	}
	std::vector<float> prevRev=std::vector<float>(this->batchSize); 
	std::vector<bool> dones= std::vector<bool>(this->batchSize, false); 
	std::vector<int> movesTaken = std::vector<int>(this->batchSize, 0);
	int active_envs = this->batchSize;
	int movesMade = 0;
	int finalVal = 0;

	while (active_envs>0 && movesMade<140)
	{
		std::vector<std::vector<GameBoard::Move>> currentBatchMoves;
		std::vector<std::vector<int>> currentBatchStates;
		std::vector<int> active_indices;
		std::vector<DummyPlayer> activeGame;
		for (int i = 0; i < this->batchSize;i++) {
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
		allStates.reserve(currentBatchStates.size() * 146);
		for (const auto& state_vector : currentBatchStates) {
			for (int val : state_vector) {
				allStates.push_back(val);
			}
		}
		torch::Tensor batchStates_cpu = torch::tensor(allStates, torch::TensorOptions().dtype(torch::kFloat32)).view({ active_envs, 146});
	
		torch::Tensor batchStates = batchStates_cpu.to(this->device);
		this->obs_normalizer.update(batchStates, this->device);

		torch::Tensor obs_mean = this->obs_normalizer.mean().to(this->device);
		torch::Tensor obs_std = this->obs_normalizer.std().to(this->device);
		torch::Tensor normalized_batchStates = (batchStates - obs_mean) / (obs_std + 1e-8);
		BatchActionProbPair outputBatch;
		torch::Tensor actionsBatch;
		if (Dummy) {
			outputBatch = getActionBatchDummy(activeGame,normalized_batchStates,currentBatchMoves);
			actionsBatch = outputBatch.actions.to(torch::kCPU);
		}
		else {
			outputBatch = getActionBatch(normalized_batchStates, currentBatchMoves);
			actionsBatch= outputBatch.actions.to(torch::kCPU).squeeze(1);
		}
		torch::Tensor probsBatch = outputBatch.log_probs.to(torch::kCPU);


		int activeIndex = 0;
		if (actionsBatch.numel() < 1) {
			for (int i = 0; i < this->batchSize; i++) {
				rewards.at(i).back() = -1000;
			}
			break;
		}
		for (int i = 0; i < this->batchSize;i++) {
			if (!dones.at(i)) {
				int action = actionsBatch[activeIndex].item<int>();
				float actionProb = probsBatch[activeIndex].item<float>();
				float baseScore = games.at(i).boardScore();
				games.at(i).doMove(GameBoard::DecodeMove(action));
				float reward = (games.at(i).boardScore() - baseScore);
				float currentRev = games.at(i).numOfCardsRevealed();
				if (action == 1 || (action >= 30 && action < 58)) {
					reward += 10.0f;
				}
				else if (prevRev.at(i) < currentRev) {
					reward += 5.0f;
				}
				else if (action == 0) {
					reward += -0.5f;
				}
				else {
					reward += -0.01f;
				}
				prevRev.at(i) = currentRev; 
				if (currentBatchStates.at(activeIndex).size() != 146) {
					std::cout << "error";

				}
				states.at(i).push_back(torch::tensor(currentBatchStates.at(activeIndex), torch::kFloat32).to(this->device));
				actions.at(i).push_back(action);
				rewards.at(i).push_back(reward);
				actionsProb.at(i).push_back(actionProb);

				if (games.at(i).isGameOver()) {
					dones.at(i) = true;
					active_envs--;
					movesTaken.at(i) = movesMade;
					if (games.at(i).gameWon()) {
						if (!Dummy) {
							//std::cout << "Winner"<<std::endl;
						}
						rewards.at(i).back() = 1000;
						this->winFlag = true;
					}
					else
						rewards.at(i).back() = -1000;
				}
				if (movesMade == 139) {
					rewards.at(i).back() = -1000;
				}
				activeIndex++;
			}
		}
		
		movesMade++;
	}
	trajectorie tra = flatten(states, actions, computeRewardsToGo(rewards), actionsProb);
	torch::Tensor adv = torch::tensor(advantageGAE(rewards, states),torch::kFloat).to(this->device);

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


std::vector<float> PPO::advantageGAE(std::vector<std::vector<float>> rewards, std::vector <std::vector<torch::Tensor>> states) {
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
	for (int i = 0; i < this->batchSize; i++) {
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
	torch::Tensor obs_mean = this->obs_normalizer.mean().to(this->device);
	torch::Tensor obs_std = this->obs_normalizer.std().to(this->device);
	torch::Tensor normalized_obs = (obs - obs_mean) / (obs_std + 1e-8);
	return this->critic.get()->forward(normalized_obs).squeeze().to(this->device);
}

torch::Tensor PPO::evaluateProb(torch::Tensor obs, torch::Tensor act) {
	torch::Tensor obs_mean = this->obs_normalizer.mean().to(this->device);
	torch::Tensor obs_std = this->obs_normalizer.std().to(this->device);
	torch::Tensor normalized_state = (obs - obs_mean) / (obs_std + 1e-8);
	torch::Tensor logs = this->actor.get()->forward(normalized_state);
	torch::Tensor probs = torch::softmax(logs, -1);      
	torch::Tensor log_probs = torch::log_softmax(logs, -1);
	log_probs = log_probs.gather(1, act.unsqueeze(1)).squeeze(1);
	return log_probs.to(this->device);
}

void PPO::LoadActor(NetDeep model,std::string path) {
	try {
		torch::load(model, path);
		std::cout << path+" model loaded successfully.\n";
	}
	catch (const c10::Error& e) {
		std::cout << path + " moded did not load.\n";
	}
}

void PPO::LoadCritic(NetDeep model, std::string path) {
	try {
		torch::load(model, path);
		std::cout << path + " model loaded successfully.\n";
	}
	catch (const c10::Error& e) {
		std::cout << path + " moded did not load.\n";
	}
}

void PPO::LogData(float totalReward, float Actorloss, float CriticLoss, float AdvantageMean, float AdvantageStd) {
	this->logger <<"," << totalReward << "," << Actorloss << "," << CriticLoss << "," << AdvantageMean<<"," << AdvantageStd << "\n";
	std::cout << totalReward << "," << Actorloss << "," << CriticLoss << "," << AdvantageMean <<"," << AdvantageStd << "\n";
}

void PPO::learn() {
	srand(time(0));
	LoadActor(this->actor,"C:/Users/jonat/source/25-26_CE301_sale_jonathan/SolitairCPPCM/Resources/actor.pth");
	LoadCritic(this->critic,"C:/Users/jonat/source/25-26_CE301_sale_jonathan/SolitairCPPCM/Resources/critic.pth");
	trajectorie winTra;
	std::cout<<"|reward|Actorloss|CriticLoss|AdvantageMean|MeanReward"<<endl;
	long double avgSumReward = 0;
	for (int j = 1; j < this->numberLoops+1; j++) {
		torch::Tensor ak;
		advantegeTrajectorie traActor = this->rollout(false);
		//trajectorie traDummy = this->rollout(true);
		torch::Tensor rewards = traActor.traject.rewards;//torch::cat({ traActor.rewards,traDummy.rewards }, 0);
		torch::Tensor actions = traActor.traject.actions;//torch::cat({ traActor.actions,traDummy.actions }, 0);
		torch::Tensor states = traActor.traject.states;//torch::cat({ traActor.states,traDummy.states }, 0);
		torch::Tensor prob = traActor.traject.actionProb;//torch::cat({ traActor.actionProb,traDummy.actionProb }, 0);


		rewards = (rewards - rewards.mean()) / (rewards.std() + 1e-8);
		avgSumReward += torch::sum(rewards).item<float>();
		ak = traActor.advantage;
		ak = (ak - ak.mean()) / (ak.std() + 1e-10);
		torch::Tensor actorLoss;
		torch::Tensor criticLoss;
		for (int i = 0; i < this->updatesPerLoop; i++) {
			torch::Tensor V = this->evaluateReward(states).to(this->device);
			torch::Tensor curPobs = this->evaluateProb(states, actions).to(this->device);
			torch::Tensor ratio = torch::exp(curPobs - prob).to(this->device);
			torch::Tensor surr1 = ratio * ak;
			torch::Tensor surr2 = torch::clamp(ratio, 1 - this->clip, 1 + this->clip).to(this->device) * ak;
			torch::Tensor logs = this->actor.get()->forward(states).to(this->device);
			torch::Tensor probs = torch::softmax(logs, -1).to(this->device);
			torch::Tensor entropy = (-probs * torch::log_softmax(logs, -1)).sum(-1).mean();
			actorLoss = (-torch::min(surr1, surr2)).to(this->device).mean() - this->entropy_co * entropy;
			this->actorOptim.zero_grad();
			actorLoss.backward({},true);
			this->actorOptim.step();
			criticLoss = torch::nn::MSELoss()(V, rewards.detach());
			this->criticOptim.zero_grad();
			criticLoss.backward({}, true);
			torch::nn::utils::clip_grad_norm_(this->critic->parameters(), 0.5);
			this->criticOptim.step();
	
		}
	/*	this->Actlr -= 0.00000005;
		static_cast<torch::optim::AdamOptions&>(this->actorOptim.param_groups()[0].options())
			.lr(this->Actlr);*/
		//this->entropy_co -= 0.000045;
		if (j % 100 == 0 && j!=0 ) {
			LogData(torch::sum(rewards).item<float>()/this->batchSize, actorLoss.item<float>(), criticLoss.item<float>(), ak.mean().item<float>(), (avgSumReward/100)/this->batchSize);
			avgSumReward = 0;
			torch::save(this->actor, "C:/Users/jonat/source/25-26_CE301_sale_jonathan/SolitairCPPCM/Resources/actor.pth");
			torch::save(this->critic, "C:/Users/jonat/source/25-26_CE301_sale_jonathan/SolitairCPPCM/Resources/critic.pth");
		}
	}
	torch::save(this->actor, "C:/Users/jonat/source/25-26_CE301_sale_jonathan/SolitairCPPCM/Resources/actor.pth");
	torch::save(this->critic, "C:/Users/jonat/source/25-26_CE301_sale_jonathan/SolitairCPPCM/Resources/critic.pth");
}

void PPO::Test() {
	LoadActor(this->actor, "C:/Users/jonat/source/25-26_CE301_sale_jonathan/SolitairCPPCM/Resources/actor.pth");
	LoadCritic(this->critic, "C:/Users/jonat/source/25-26_CE301_sale_jonathan/SolitairCPPCM/Resources/critic.pth");
	int winCount=0;
	for (int i = 0; i < 1000;i++) {
		KlondikeGameController game(0);
		while (!game.isGameOver())
		{
			torch::Tensor curState = torch::tensor(game.EncodeBoard(), torch::kFloat32).to(this->device);
			this->obs_normalizer.update(curState,this->device);
			ActionProbPair action = getAction(curState, game.getAvailableMoves());
			game.doMove(GameBoard::DecodeMove(action.action));
			
		}
		if (game.gameWon()) {
			winCount++;

		}
	}
	std::cout << "Games Won: " << winCount << endl;
	std::cout << "This is a Win rate of:" << (winCount / (float)1000) * 100 << "%" << endl;
}