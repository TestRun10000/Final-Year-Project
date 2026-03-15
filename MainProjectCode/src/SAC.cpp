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

BatchActionProbPair SAC::getActionBatchDummy(std::vector<DummyPlayer> games, torch::Tensor states, std::vector<std::vector <GameBoard::Move>> moves) {
	std::vector<int> actions = std::vector<int>();
	int N = states.size(0);
	for (auto game : games) {
		actions.push_back(GameBoard::EncodeMove(game.bestMoveRandom()));
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



std::vector<trajectorie> SAC::rollout(int batchSize,bool dummy){
	std::vector<DummyPlayer> games= std::vector<DummyPlayer>(batchSize);
	std::vector<trajectorie> trajectories = std::vector<trajectorie>();
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
		if (dummy) {
			outputBatch = getActionBatchDummy(activeGame, batchStates, currentBatchMoves);
			actionsBatch = outputBatch.actions.to(torch::kCPU);
		}
		else {
			outputBatch = getActionBatch(batchStates, currentBatchMoves);
			actionsBatch = outputBatch.actions.to(torch::kCPU).squeeze(1);
		}
		torch::Tensor probsBatch = outputBatch.log_probs.to(torch::kCPU);


		int activeIndex = 0;
		for (int i = 0; i < batchSize;i++) {
			if (!dones.at(i)) {
				int action = actionsBatch[activeIndex].item<int>();
				float actionProb = probsBatch[activeIndex].item<float>();
				float baseScore = games.at(i).boardScore();
				torch::Tensor state = torch::tensor(games.at(i).EncodeBoard(), torch::kFloat32).to(this->device);
				games.at(i).doMove(GameBoard::DecodeMove(action));
				torch::Tensor nextState = torch::tensor(games.at(i).EncodeBoard(), torch::kFloat32).to(this->device);
				//float reward = (games.at(i).boardScore() - baseScore)/10;
				//reward = std::signbit(reward) ? -0.2:0.2;
				float reward=0.0;
				float currentRev = games.at(i).numOfCardsRevealed();
				if (action == 1 || (action >= 8 && action < 14)) {//to foundation
					reward += 1.0f;
				}
				else if (action >= 14 && action < 17){// removed card from deck
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
				//torch::Tensor normalized_state = (curState - obs_mean) / (obs_std + 1e-8);
				actionsTaken++;
				rewardMean += reward;
				bool isDone = games.at(i).isGameOver();
				if (isDone) {
					dones.at(i) = true;
					active_envs--;
					movesTaken.at(i) = movesMade;
					if (games.at(i).gameWon()) {
						winCount++;
						
						reward = 10;
						
					}
					else {
						reward  = -5;
					}
				}
				else if (movesMade == 139) {
					reward = -5;
					movesTaken.at(i) = movesMade;
				}
				//torch::Tensor vs = getQvalue(state);
				//torch::Tensor td = vs + this->alpha * (reward + this->gamma * getQvalue(nextState) - vs);
				trajectories.push_back(trajectorie(state, action, torch::tensor(reward,torch::kFloat), nextState, isDone,this->device));
				activeIndex++;
			}
		}
		movesMade++;
	}

	return trajectories;
}


torch::Tensor SAC::getQvalue(torch::Tensor state) {
	return torch::min(this->criticA.get()->forward(state), this->criticB.get()->forward(state));
}


trajectorieTensorBatch SAC::getSample(int size) {
	vector<torch::Tensor> state = vector<torch::Tensor>(size);
	vector<torch::Tensor> reward = vector<torch::Tensor>(size);
	vector<torch::Tensor> nextState = vector<torch::Tensor>(size);
	vector<torch::Tensor> action = vector<torch::Tensor>(size);
	vector<torch::Tensor> done = vector<torch::Tensor>(size);
	int i = 0;
	for (auto a : this->buffer.getSample(size)) {
		state.at(i) = a->state;
		reward.at(i) = a->reward;
		nextState.at(i) = a->nextState;
		done.at(i) = a->done;
		action.at(i) = a->action;
		i++;
	}
	return trajectorieTensorBatch(state, action, reward, nextState, done);
}




LossPair SAC::getCriticLoss(trajectorieTensorBatch tra) {
	//torch::NoGradGuard no_grad;
	torch::Tensor raw = this->actor.get()->forward(tra.nextState).detach();
	torch::Tensor actionProb = torch::softmax(raw, -1).to(this->device);
	torch::Tensor actionProbLog = torch::log_softmax(raw, -1).to(this->device);
	torch::Tensor next_q_values_target = this->criticTA.get()->forward(tra.nextState);
	torch::Tensor next_q_values_target2 = this->criticTB.get()->forward(tra.nextState);
		
	torch::Tensor soft_state_values =torch::sum(actionProb * (torch::min(next_q_values_target, next_q_values_target2) - this->alpha * actionProbLog), 1);
	torch::Tensor not_done = (1 - tra.done.to(torch::kFloat32));
	torch::Tensor nextQ = tra.reward + not_done * this->gamma * soft_state_values;
	nextQ=nextQ.detach();

	torch::Tensor soft_q = this->criticA.get()->forward(tra.state).gather(1, tra.action.unsqueeze(-1)).squeeze(-1);
	torch::Tensor soft_q2 = this->criticB.get()->forward(tra.state).gather(1, tra.action.unsqueeze(-1)).squeeze(-1);

	torch::nn::MSELoss mse_loss(torch::nn::MSELossOptions().reduction(torch::kNone));
	torch::Tensor critic_error = mse_loss(soft_q, nextQ);
	torch::Tensor critic_error2 = mse_loss(soft_q2, nextQ);
	
	std::vector<float> newWeights;
	newWeights.reserve(critic_error.size(0));
	torch::Tensor td1 = (soft_q - nextQ).detach().abs();
	torch::Tensor td2 = (soft_q2 - nextQ).detach().abs();
	torch::Tensor prio = torch::min(td1, td2);
	prio = torch::clamp(prio, 0.001f, 1.0f);
	for (int i = 0; i < critic_error.size(0); i++) {
		newWeights.push_back(prio[i].item<float>());
	}
	this->buffer.updateWeights(newWeights);

	torch::Tensor criticLossA = critic_error.mean();
	torch::Tensor criticLossB = critic_error2.mean();
	return LossPair(criticLossA,criticLossB);
}

torch::Tensor SAC::getActorLoss(trajectorieTensorBatch tra) {
	torch::Tensor raw = this->actor.get()->forward(tra.state);
	torch::Tensor actionProb = torch::softmax(raw, -1).to(this->device);
	torch::Tensor actionProbLog = torch::log_softmax(raw, -1).to(this->device);

	torch::Tensor q_values_local = this->criticA.get()->forward(tra.state).detach();
	torch::Tensor q_values_local2 = this->criticB.get()->forward(tra.state).detach();
	torch::Tensor inside_term =  -torch::min(q_values_local, q_values_local2)+this->alpha * actionProbLog;

	torch::Tensor policyLoss = torch::sum((actionProb * inside_term), 1).mean();

	return -policyLoss;
}

torch::Tensor SAC::getEntropyLoss(trajectorieTensorBatch tra) {
	torch::Tensor raw = this->actor->forward(tra.state).detach();
	torch::Tensor log_prob = torch::log_softmax(raw, -1);
	torch::Tensor prob = torch::softmax(raw, -1);
	torch::Tensor expected_logp = (prob * log_prob).sum(1);  // shape [batch]
	return -(this->logAlpha * (expected_logp.detach() + this->target_entropy)).mean();
}



void SAC::LoadNetwork(NetDeep model,std::string path) {
	try {
		torch::load(model, path);
		std::cout << path+" model loaded successfully.\n";
		this->newNet = false;
	}
	catch (const c10::Error& e) {
		std::cout << path + " moded did not load.\n";
	}
}


void SAC::LogData(float Actorloss, float CriticLoss,float MeanReward, float entropyLoss) {
	this->logger <<"," << Actorloss << "," << CriticLoss << ",";
	this->logger << MeanReward << "," << this->winCount << "," << ((float)this->winCount / ((float)this->rolloutBatchSize * 10)) * 100 << "," << entropyLoss << "\n";
	std::cout << Actorloss << "," << CriticLoss << ",";
	std::cout << MeanReward << "," << this->winCount << "," << ((float)this->winCount / ((float)this->rolloutBatchSize * 10)) * 100 << "," << entropyLoss;
	std::cout<<","<<this->alpha.item<float>()<<","<<this->logAlpha.item<float>() << "\n";
}

void SAC::learn() {
	trajectorie winTra;
	//std::cout<<"|reward2Go|Actorloss|CriticLoss|AdvantageMean|MeanReward|KL|EntropyMean|Explained Var|Win Count"<<endl;
	std::cout<<"Actorloss|CriticLoss|MeanReward|Win Count|Win Percentage|EntropyLoss|Entropy"<<endl;
	long double avgSumReward = 0;
	for (int j = 1; j < this->numberLoops+1; j++) {
		srand(j%200);
		float meanReward = 0.0;
		int k = 0;
		for (auto tra : this->rollout(this->rolloutBatchSize)) {
			this->buffer.push(tra);
			meanReward += tra.reward.item<float>();
			k++;
		}
		meanReward /= this->batchSize;
		float actorLossVal = 0.0;
		float criticLossVal = 0.0;
		float entropyLossVal = 0.0;
		if (this->buffer.getSize() >= this->batchSize) {
			for (int i = 0; i < this->updatesPerLoop; i++) {
				//std::cout << "done" << std::endl;
				trajectorieTensorBatch sample = getSample(this->batchSize);
				LossPair criticLoss = getCriticLoss(sample);
				torch::Tensor actorLoss = getActorLoss(sample);
				torch::Tensor entropyLoss = getEntropyLoss(sample);
				this->criticAOp.zero_grad();
				this->criticBOp.zero_grad();
				this->actorOptim.zero_grad();
				this->alphaOp.zero_grad();
				criticLoss.lossA.backward();
				this->criticAOp.step();
				criticLossVal = torch::min(criticLoss.lossA, criticLoss.lossB).item<float>();
				
				criticLoss.lossB.backward();
				this->criticBOp.step();

				actorLossVal = actorLoss.item<float>();
				actorLoss.backward();
				this->actorOptim.step();
				
				entropyLossVal = entropyLoss.item<float>();
				entropyLoss.backward();
				this->alphaOp.step();
				this->alpha = this->logAlpha.exp();
				soft_update_target_networks(this->tau);
			}
		}

		if (j % 10 == 0 ||(j<=10 && this->newNet)) {
			LogData(actorLossVal, criticLossVal,meanReward,entropyLossVal );
			this->winCount = 0;
			torch::save(this->actor, (std::string)RESOURCES_PATH + "actor.pth");
			torch::save(this->criticA, (std::string)RESOURCES_PATH + "criticA.pth");
			torch::save(this->criticB, (std::string)RESOURCES_PATH + "criticB.pth");

		}
		if (j % 100 == 0) {
			//std::cout << "Clear" << std::endl;
			this->logAlpha = torch::tensor(std::log(0.1), torch::TensorOptions().dtype(torch::kFloat32).requires_grad(true));
			this->alpha = logAlpha.exp();
			this->buffer.clearBuffer();
		}
	}

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