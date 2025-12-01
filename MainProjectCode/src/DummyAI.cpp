#include"DummyAI.h"
#include<torch/torch.h>



GameBoard::Move DummyAI::makeBestMove() {
	vector<KlondikeGameController::Move> moves = getAvailableMoves();
	std::priority_queue<DummyPlayer::ActionValue, std::vector<DummyPlayer::ActionValue>, CompareTask> task_queue;
	for (KlondikeGameController::Move m : moves) {
		GameBoard copy = DummyPlayer::copyBoard();
		DummyPlayer::doMove(m, &copy);
		float score = 0;
		if (this->criticLoaded) {
			torch::Tensor obs = torch::tensor(copy.EncodeBoard(), torch::TensorOptions().dtype(torch::kFloat32)).to(torch::kCUDA);
			torch::Tensor out = DummyAI::critic.get()->forward(obs);
			score=out.to(torch::kCPU).item<float>();
		}
		else {
			score = this->bso.boardScore(copy);
		}
		task_queue.emplace(score, m);
	}
	/*KlondikeGameController::doMove(task_queue.top().move);*/
	return task_queue.top().move;
}